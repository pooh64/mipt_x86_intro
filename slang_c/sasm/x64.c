#include "database.h"
#include "../public/sasm_codes.h"
#include "x64.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>

int sasm_calc_labels_x64(struct sasm_t *dbase, size_t *size);
int sasm_gen_x64(struct sasm_t *dbase, unsigned char *buf);
int sasm_asm_x64(struct sasm_t *dbase, char **buf, size_t *size);

#define APPEND_SHELLC(strarr)			\
do {						\
	memcpy(buf + offs, strarr, 		\
	sizeof(strarr) - 1);			\
	offs += sizeof(strarr) - 1;		\
} while(0);

#define APPEND_BYTE(val)			\
do {						\
	((unsigned char*) buf)[offs] = val;	\
	offs += sizeof(char);			\
} while(0);

#define APPEND_INT(val)				\
do {						\
	*(int32_t*) (buf + offs) = val;		\
	offs += sizeof(int32_t);		\
} while(0);

#define GET_RELOFFS_x64(labelptr)		\
(int32_t) labelptr->offs - offs - sizeof(int32_t)

#define x64_ERR_MSG 				\
fprintf(stderr, "Error: %s: unknown syntax\n\t\
type: %d\n\t\
val:  %" PRId32 "\n\t\
offs: %zd\n", __FUNCTION__, cur->type, cur->val, offs);


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


int sasm_gen_x64(struct sasm_t *dbase, unsigned char *buf)
{
	size_t offs = 0;
	FILE *ext = 0;
	size_t extsize = 0;
	for(struct elem_t *cur = dbase->code; \
	cur->type != SASM_TYPE_NULL; cur = cur->next) {

		if(cur->type == SASM_TYPE_LABEL)
			continue;
			
		if (cur->type == SASM_TYPE_EXTERN) {
			cur->label->offs = offs;
			ext = fopen(cur->label->name, "r");
			if (ext == NULL) {
				fprintf(stderr, \
				"Error: Can't open extern file \
				%s.20:", cur->label->name);
				perror(" ");
				return -1;
			}
			if (get_fsize(ext, &extsize) == -1)
				return -1;
			if(extsize != fread(buf + offs, sizeof(char), extsize, ext)) {
				perror("Error: fread failed: ");
				fclose(ext);
				return -1;
			}
			offs += extsize;
			fclose(ext);
			continue;
		}

		if(cur->type != SASM_TYPE_CMD) {
			x64_ERR_MSG;
			return -1;
		}

		switch(cur->val) {
		case SASM_CMD_call:
			APPEND_BYTE(x64_call);
			cur = cur->next;
			APPEND_INT(GET_RELOFFS_x64(cur->label));
			break;
		case SASM_CMD_ret:
			APPEND_BYTE(x64_ret);
			break;
		case SASM_CMD_getrsp:
			APPEND_SHELLC(x64_getrsp);
			break;
		case SASM_CMD_setrsp:
			APPEND_SHELLC(x64_setrsp);
			break;
		case SASM_CMD_pushd:
			APPEND_BYTE(x64_pushd);
			cur = cur->next;
			APPEND_INT((int32_t) cur->val);
			break;
		case SASM_CMD_pushr:
			cur = cur->next;
			APPEND_BYTE(x64_pushr + (unsigned char) cur->val);
			break;
		case SASM_CMD_popr:
			cur = cur->next;
			APPEND_BYTE(x64_popr + (unsigned char) cur->val);
			break;
		case SASM_CMD_pushm:
			APPEND_BYTE(x64_pushm[0]);
			cur = cur->next;
			APPEND_BYTE(x64_pushm[1] + (unsigned char) cur->val);
			if((unsigned char) cur->val == SASM_REG_rsp)
				APPEND_BYTE(x64_rsp_byte);
			cur = cur->next;
			APPEND_INT((int32_t) cur->val);
			break;
		case SASM_CMD_popm:
			APPEND_BYTE(x64_popm[0]);
			cur = cur->next;
			APPEND_BYTE(x64_popm[1] + (unsigned char) cur->val);
			if((unsigned char) cur->val == SASM_REG_rsp)
				APPEND_BYTE(x64_rsp_byte);
			cur = cur->next;
			APPEND_INT((int32_t) cur->val);
			break;

#define CASE_CMD_ARITHM(name)					\
		case SASM_CMD_##name:				\
			APPEND_SHELLC(x64_stack_##name);	\
			break;

		CASE_CMD_ARITHM(add);
		CASE_CMD_ARITHM(sub);
		CASE_CMD_ARITHM(mul);
		CASE_CMD_ARITHM(div);
#undef CASE_CMD_ARITHM

		case SASM_CMD_jmp:
			APPEND_BYTE(x64_jmp);
			cur = cur->next;
			APPEND_INT(GET_RELOFFS_x64(cur->label));
			break;

#define CASE_CONDIT_JMP(name)						\
		case SASM_CMD_##name:					\
			APPEND_SHELLC(x64_stack_cmp);			\
			APPEND_SHELLC(x64_##name);			\
			cur = cur->next;				\
			APPEND_INT(GET_RELOFFS_x64(cur->label));	\
			break;

		CASE_CONDIT_JMP(je);
		CASE_CONDIT_JMP(jne);
		CASE_CONDIT_JMP(jg);
		CASE_CONDIT_JMP(jl);
		CASE_CONDIT_JMP(jge);
		CASE_CONDIT_JMP(jle);
#undef CASE_CONDIT_JMP

		default:
			x64_ERR_MSG;
			return -1;
		}
	}
	return 0;
}



#undef APPEND_SHELLC
#undef APPEND_BYTE
#undef APPEND_INT
#undef GET_RELOFFS_x64

// Redefine + copy = best solution

#define APPEND_SHELLC(strarr)			\
do {						\
	offs += sizeof(strarr) - 1;		\
} while(0);

#define APPEND_BYTE(val)			\
do {						\
	offs += sizeof(char);			\
} while(0);

#define APPEND_INT(val)				\
do {						\
	offs += sizeof(int32_t);		\
} while(0);

#define GET_RELOFFS_x64(labelptr) 0




int sasm_calc_labels_x64(struct sasm_t *dbase, size_t *size)
{
	size_t offs = 0;
	FILE *ext = NULL;
	size_t extsize = 0;
	for(struct elem_t *cur = dbase->code; \
	cur->type != SASM_TYPE_NULL; cur = cur->next) {

		if (cur->type == SASM_TYPE_LABEL) {
			cur->label->offs = offs;
			continue;
		}
		
		if (cur->type == SASM_TYPE_EXTERN) {
			cur->label->offs = offs;
			ext = fopen(cur->label->name, "r");
			if (ext == NULL) {
				fprintf(stderr, \
				"Error: Can't open extern file \
				%s.20:", cur->label->name);
				perror(" ");
				return -1;
			}
			if (get_fsize(ext, &extsize) == -1)
				return -1;
			offs += extsize;
			fclose(ext);
			continue;
		}

		if(cur->type != SASM_TYPE_CMD) {
			x64_ERR_MSG;
			return -1;
		}

		switch(cur->val) {
		case SASM_CMD_call:
			APPEND_BYTE(x64_call);
			cur = cur->next;
			APPEND_INT(GET_RELOFFS_x64(cur->label));
			break;
		case SASM_CMD_ret:
			APPEND_BYTE(x64_ret);
			break;
		case SASM_CMD_getrsp:
			APPEND_SHELLC(x64_getrsp);
			break;
		case SASM_CMD_setrsp:
			APPEND_SHELLC(x64_setrsp);
			break;
		case SASM_CMD_pushd:
			APPEND_BYTE(x64_pushd);
			cur = cur->next;
			APPEND_INT((int32_t) cur->val);
			break;
		case SASM_CMD_pushr:
			cur = cur->next;
			APPEND_BYTE(x64_pushr + (unsigned char) cur->val);
			break;
		case SASM_CMD_popr:
			cur = cur->next;
			APPEND_BYTE(x64_popr + (unsigned char) cur->val);
			break;
		case SASM_CMD_pushm:
			APPEND_BYTE(x64_pushm[0]);
			cur = cur->next;
			APPEND_BYTE(x64_pushm[1] + (unsigned char) cur->val);
			if((unsigned char) cur->val == SASM_REG_rsp)
				APPEND_BYTE(x64_rsp_byte);
			cur = cur->next;
			APPEND_INT((int32_t) cur->val);
			break;
		case SASM_CMD_popm:
			APPEND_BYTE(x64_popm[0]);
			cur = cur->next;
			APPEND_BYTE(x64_popm[1] + (unsigned char) cur->val);
			if((unsigned char) cur->val == SASM_REG_rsp)
				APPEND_BYTE(x64_rsp_byte);
			cur = cur->next;
			APPEND_INT((int32_t) cur->val);
			break;

#define CASE_CMD_ARITHM(name)					\
		case SASM_CMD_##name:				\
			APPEND_SHELLC(x64_stack_##name);	\
			break;

		CASE_CMD_ARITHM(add);
		CASE_CMD_ARITHM(sub);
		CASE_CMD_ARITHM(mul);
		CASE_CMD_ARITHM(div);
#undef CASE_CMD_ARITHM

		case SASM_CMD_jmp:
			APPEND_BYTE(x64_jmp);
			cur = cur->next;
			APPEND_INT(GET_RELOFFS_x64(cur->label));
			break;

#define CASE_CONDIT_JMP(name)						\
		case SASM_CMD_##name:					\
			APPEND_SHELLC(x64_stack_cmp);			\
			APPEND_SHELLC(x64_##name);			\
			cur = cur->next;				\
			APPEND_INT(GET_RELOFFS_x64(cur->label));	\
			break;

		CASE_CONDIT_JMP(je);
		CASE_CONDIT_JMP(jne);
		CASE_CONDIT_JMP(jg);
		CASE_CONDIT_JMP(jl);
		CASE_CONDIT_JMP(jge);
		CASE_CONDIT_JMP(jle);
#undef CASE_CONDIT_JMP

		default:
			x64_ERR_MSG;
			return -1;
		}
	}
	*size = offs;
	return 0;
}


int sasm_asm_x64(struct sasm_t *dbase, char **buf, size_t *size)
{
	if (sasm_calc_labels_x64(dbase, size) == -1)
		return -1;
	*buf = (char*) calloc(sizeof(char), *size);
	if (*buf == NULL)
		return -1;
	if (sasm_gen_x64(dbase, (unsigned char*) *buf) == -1) {
		free(buf);
		return -1;
	}
	return 0;
}