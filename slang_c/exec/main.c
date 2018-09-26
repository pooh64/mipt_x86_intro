#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

extern char x64_code[];

asm(	
	"x64_code:\n"
	".incbin \"./debug/test/code.x64\"\n"
);

int main()
{
	int64_t (*code)() = (int64_t (*)()) x64_code;
	int64_t rax = (*code)();
	printf("Process returned %" PRId64 "\n", rax);
	return 0;
}

