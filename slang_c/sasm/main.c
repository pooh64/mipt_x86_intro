#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "database.h"

int dumpbuf(char *buf, size_t bufsize)
{
	for (size_t i = 0; i < bufsize; i++)
		printf("[%zd]%d\n", i, buf[i]);
	return 0;
}

int dump_elem(struct elem_t *elem)
{
	while (elem != NULL) {
		printf("type:%2d val:%2zx\n", elem->type, elem->val);
		elem = elem->next;
	}
	return 0;
}

int file_get_buf(FILE *file, char **buf, size_t *size)
{
	fseek(file, 0, SEEK_END);
	if (errno)
		return -1;
	*size = ftell(file) + 1;
	if (errno)
		return -1;
	fseek(file, 0, SEEK_SET);
	if (errno)
		return -1;
	*buf = (char*) calloc(*size, sizeof(char));
	if (*buf == NULL)
		return -1;
	fread(*buf, sizeof(char), *size, file);
	if (errno)
		return -1;
	return 0;
}

int main()
{
	// Preparing files
	FILE *inp  = fopen("./test/sasm.txt", "r");
	FILE *outp_x64 = fopen("./test/code.x64", "w");
	if(!inp) {
		fprintf(stderr, "Error: Can't open input file (./test/sasm.txt)\n");
		return -1;
	}
	if(!outp_x64) {
		fprintf(stderr, "Error: Outp file creating error\n");
		return -1;
	}

	char *buf = NULL;
	size_t buf_size = 0;

	// Making sasm-database from file
	if(file_get_buf(inp, &buf, &buf_size) == -1) {
		fprintf(stderr, "Error: File reading failed\n");
		return -1;
	}
	struct sasm_t *dbase = calloc(sizeof(struct sasm_t), 1);
	if(dbase == NULL) {
		fprintf(stderr, "%s\n", "Error: Calloc failed\n");
		return -1;
	}
	if(sasm_parse(dbase, buf) == -1) {
		fprintf(stderr, "Error: Parse failed\n");
		free(buf);
		return -1;
	}
	free(buf);
	if(sasm_verify_codelist(dbase->code) == -1) {
		fprintf(stderr, "Error: Corrupted codelist\n");
		return -1;
	}
	if(sasm_verify_labellist(dbase->label) == -1) {
		fprintf(stderr, "Error: Corrupted labellist\n");
		return -1;
	}

	/// Creating x64-code
	if(sasm_asm_x64(dbase, &buf, &buf_size) == -1) {
		fprintf(stderr, "Error: x64-code creating failed\n");
		return -1;
	}
	fwrite(buf, sizeof(char), buf_size, outp_x64);
	free(buf);

	sasm_delete_codelist(dbase->code);
	sasm_delete_labellist(dbase->label);
	free(dbase);
	fclose(inp);
	fclose(outp_x64);
	return 0;
}
