# slang
The c-like programming language, its ASM, JIT, VM

ssc - slang compiler, translates .sl to .sasm
	.sasm contents slang asm code

sasm - slang bytecode generator
	bytecode can be executed by sjit and svm
	
sjit - slang bytecode JIT-compiler, also produce
out file with copy of mmap binary buffer

svm - slang VM, executes slang bytecode


slang syntax:
	in fact_example you can find short example

/// function definition:
long funcname();

/// function implementation:
long funcname()
{
	/// local varialbe definition
	/// local var's in stack
	/// notice: variables can't be filled with some falue in init
	/// better to write it on the top (better codestyle for funccalls)
	long a;
	long b;
	long c;
	
	/// notice: first 3 defined variables in this func will be filled with 1, 2, 3
	/// 	other will be pushed with 0
	/// 	i.e. pushq 1; pushq 2; pushq 3; pushq 0; ... pushq 0; 	then call
	/// called func saves rbp, returns value in rax
	/// notice: anotherfunc(1, 2, 3) w/o assignment is banned (because nobody pops it from stack)
	a = anotherfunc(1, 2, 3);
	/// caller must clean stack now
	/// i.e. popq bx; ... ; popq bx;
	/// pushq rax;
	
	/// any expression w/o if/while/assignment causes indeterminate behavior
	/// because result willn't be pop'd from stack, it's val can be used as retaddr 
	/// = wrong result, then 99.999999% segmentation fault
	a = a + 3 + 2 * somefunc(15, 2) / c + d;
	
	/// c-like expressions, this line is equal to a = 2;
	a = 2 > 1 + 1;
	
	/// while has same syntax
	/// notice: ';' at the end
	if( expression )
	{
		code
	};
	
	/// notice: w/o return there will be no ret
	return a;
}

/// always run first (defined as __process_0 in sasm)
/// retvalue of main is returned like retval of all process
long main()
{
	return 0;
}




