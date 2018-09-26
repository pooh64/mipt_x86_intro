#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "database.h"
#include "../public/sasm_codes.h"

struct label_t *sasm_find_label(struct sasm_t *dbase, char *str)
{
	struct label_t *tmp = dbase->label;
	while (tmp != NULL) {
		if (!strcmp(tmp->name, str))
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

/// char *str will be used in the new element of label_list
struct label_t *sasm_add_label(struct sasm_t *dbase, char *str)
{
	struct label_t *tmp = dbase->label;
	struct label_t *newlabel = calloc(sizeof(struct label_t), 1);
	if (newlabel == NULL)
		return NULL;
	newlabel->name = str;
	if (tmp == NULL)
		dbase->label = newlabel;
	else {
		while(tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = newlabel;
	}
	return newlabel;
}

int sasm_delete_codelist(struct elem_t *elem)
{
	struct elem_t *tmp = NULL;
	while (elem != NULL) {
		tmp = elem->next;
		free(elem);
		elem = tmp;
	}
	return 0;
}

int sasm_delete_labellist(struct label_t *label)
{
	struct label_t *tmp = NULL;
	while (label != NULL) {
		tmp = label->next;
		if(label->name != NULL)
			free(label->name);
		free(label);
		label = tmp;
	}
	return 0;
}


int sasm_dump_elem(struct elem_t *elem)
{
	switch(elem->type) {
	case SASM_TYPE_NULL:
		printf("TYPE_NULL\n");
		break;

	case SASM_TYPE_CMD:
		if(0);
#define CMD(name) else if (elem->val == SASM_CMD_##name) printf("%s\n", #name);
#include "../public/sasm_cmds.h"
#undef CMD
		break;

	case SASM_TYPE_REG:
		if(0);
#define REG(name) else if (elem->val == SASM_REG_##name) printf("%s\n", #name);
#include "../public/sasm_regs.h"
#undef REG
		break;	

	case SASM_TYPE_DW:
		printf("%" PRId32 "\n", elem->val);
		break;
	
	case SASM_TYPE_RELOFFS:
		printf("TYPE_RELOFFS:\n");
		if (elem->label == NULL) { 
			printf("\tError: NULL-label pointer\n");
			break;
		}
		if (elem->label->ptr == NULL)
			printf("\tWarning: NULL-elem linked to label\n");
		else 
			printf("Elem linked to label: %p\n", elem->label->ptr);
		printf("Label %p", elem->label);
		if (elem->label->name == NULL)
			printf("\n\tWarning: noname-label\n");
		else
			printf(" (%s)\n", elem->label->name);
		printf("Offs: %" PRId32 "\n\n", elem->label->offs);
		break;
	
	case SASM_TYPE_LABEL:
		printf("TYPE_LABEL:\n");
		if (elem->label == NULL) { 
			printf("\tError: NULL-label pointer\n");
			break;
		}
		if (elem->label->ptr == NULL)
			printf("\tWarning: NULL-elem linked to label\n");
		else 
			printf("Elem linked to label: %p\n", elem->label->ptr);
		printf("Label %p", elem->label);
		if (elem->label->name == NULL)
			printf("\n\tWarning: noname-label\n");
		else
			printf(" (%s)\n", elem->label->name);
		printf("Offs: %" PRId32 "\n\n", elem->label->offs);
		break;

	case SASM_TYPE_EXTERN:
		printf("TYPE_LABEL:\n");
		if (elem->label == NULL) { 
			printf("\tError: NULL-label pointer\n");
			break;
		}
		if (elem->label->ptr == NULL)
			printf("\tWarning: NULL-elem linked to label\n");
		else 
			printf("Elem linked to label: %p\n", elem->label->ptr);
		printf("Label %p", elem->label);
		if (elem->label->name == NULL)
			printf("\n\tWarning: noname-label\n");
		else
			printf(" (%s)\n", elem->label->name);
		printf("Offs: %" PRId32 "\n\n", elem->label->offs);
		break;
	
	default:
		printf("\tWarning: Unknown elem\ntype: %d\nval: %" PRId32 "\n"\
		, elem->type, elem->val);
		break; 
	}
}


int sasm_dump_codelist(struct elem_t *elem)
{
	printf("CODELIST DUMP\\\n");
	for(; elem != NULL; elem = elem->next)
		sasm_dump_elem(elem);
	printf("\\DUMP\n");
	return 0;
}

int sasm_verify_labellist(struct label_t *label)
{
	if (label == NULL)
		return -1;
	for (struct label_t *cur = label; cur != NULL; cur = cur->next) {
		if (cur->ptr == NULL) {
			fprintf(stderr, "Error: Label ptr undefined (%s)\n", \
			cur->name ? cur->name : "name undefined");
			return -1;
		}
		if (cur->name == NULL) {
			fprintf(stderr, "Error: Label name undefined\n");
			return -1;
		}
	}
	return 0;
}

int sasm_verify_codelist(struct elem_t *elem)
{
	if (elem == NULL)
		return -1;
	for (; elem != NULL; elem = elem->next) {
		switch (elem->type) {
		case SASM_TYPE_NULL:
		case SASM_TYPE_DW:
			break;

		case SASM_TYPE_CMD:
#define CMD(name) else if (elem->val == SASM_CMD_##name);
		if (0);
#include "../public/sasm_cmds.h"
#undef CMD
		else {
			fprintf(stderr, "Error: Wrong cmd code (%" PRId32 ")\n", elem->val);
			return -1;
		}
			break;

		case SASM_TYPE_REG:
#define REG(name) else if (elem->val != SASM_REG_##name);
		if (0);
#include "../public/sasm_regs.h"
#undef REG
		else {
			fprintf(stderr, "Error: Wrong reg code (%" PRId32 ")\n", elem->val);
			return -1;
		}
			break;

		case SASM_TYPE_RELOFFS:
		case SASM_TYPE_LABEL:
		case SASM_TYPE_EXTERN:
			if (elem->label == NULL) {
				fprintf(stderr, "Error: Unlinked label\n");
				return -1;
			}
			break;

		default:
			fprintf(stderr, "Error: Unknown type in codelist (%d)\n", elem->type);
			return -1;
		}

		if (elem->type == SASM_TYPE_NULL && elem->next != NULL) {
			fprintf(stderr, "Error: TYPE_NULL not at the end\n");
			return -1;
		}
	}
	return 0;	
}
