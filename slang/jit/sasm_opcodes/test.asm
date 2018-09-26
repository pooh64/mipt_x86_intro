global  _pushq
section .text

getrbp_:
	push rbp
	mov rbp, rsp
	add rbp, 16
	
retrbp_:
	pop rbp
	
addq:
	pop rbx
	pop rax
	add rax, rbx
	push rax
	
subq:
	pop rbx
	pop rax
	sub rax, rbx
	push rax
	
mulq:
	pop rbx
	pop rax
	mul rbx
	push rax
	
divq:
	pop rbx
	pop rax
	div rbx
	push rax


pushq_:
	push rbp
	push rax
	push rbx
	push qword [rbp]
	push qword 1
	push qword 1000
	push qword 1000000
	push qword 0fffffffh
	
popq_:
	pop qword rax
	pop qword rbx
	pop qword rbp
	pop qword [rbp]
	
	lea rax, [$]
	lea rax, [0]
	
jmp_:
	mov rdx, rcx
	add rdx, 0fffffffh
	jmp rdx
	jmp 4
	
jcc_:
	ja 11000
	ja 29932
	ja 1889
	ja 19931
	pop rbx
	pop rax
	cmp rax, rbx
	ja 100000
	nop
	pop rbx
	pop rax
	cmp rax, rbx
	jb 100000
	nop
	pop rbx
	pop rax
	cmp rax, rbx
	je 100000
	nop
	pop rbx
	pop rax
	cmp rax, rbx
	jne 100000
	nop

call:
	mov rdx, rcx
	add rdx, 0f54ffh
	call rdx
	call 100d0h
	
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
		nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lol_:
	nop
	lol_1:
	nop
	
	
	
	ret

	
	