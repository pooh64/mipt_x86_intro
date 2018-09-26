#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

int get_fsize(FILE *file, size_t *size)
{
	fseek(file, 0, SEEK_END);
	if (errno)
		return -1;
	*size = ftell(file);
	if (errno)
		return -1;
	fseek(file, 0, SEEK_SET);
	if (errno)
		return -1;
	return 0;
}

int main()
{
	FILE *codefile = fopen("./test/code.x64", "r");
	if (errno) {
		perror("Can't open file ./test/x64.txt");
		return 0;
	}

	size_t codesize = 0;
	if(get_fsize(codefile, &codesize) == -1) {
		perror("Fileuse failed");
		return 0;
	}

	char *mem = mmap (NULL, codesize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (errno) {
		perror("mmap failed");
		return 0;
	}

	
	fread(mem, sizeof(char), codesize, codefile);
	if (errno) {
		perror ("fread failed");
		return 0;
	}

	
	int64_t retval = (* (int64_t (*)()) mem)();
	printf("Process returned %" PRId64 "d (0x%" PRIx64 ")\n", retval, retval); 

	munmap(mem, codesize);
	return 0;
}
