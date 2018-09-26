#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include "../public/serrno.h"
#include "../public/sasm_codes.h"
#include "database.h"

// Function list
int sasm_parse(struct sasm_t *dbase, char *text);
int parse_detect_separated(char **text, char *token);
int parse_detect_unsepar(char **text, char *token);
int get_command(char **text, struct elem_t **elem);
int get_register(char **text, struct elem_t **elem);
int get_addressing(char **text, struct elem_t **elem);
int get_integer(char **text, struct elem_t **elem);
int get_label(char **text, struct elem_t **elem, struct sasm_t *sasm);
int get_name(char **text, struct elem_t **elem, struct sasm_t *sasm);
int get_direct(char **text, struct elem_t **elem, struct sasm_t *sasm);

int read_name(char *text, char **name, size_t *namlen);
int read_cstr(char **text, char **name, size_t *namlen);

//#define DUMP() printf("%s: %.10s\n", __FUNCTION__, *text);
#define DUMP() ;

/// Main parse function
int sasm_parse(struct sasm_t *dbase, char *text)
{
	if(dbase->code != NULL)
		sasm_delete_codelist(dbase->code);
	struct elem_t *elem = (struct elem_t*)	\
			      calloc(sizeof(struct elem_t), 1);
	dbase->code = elem;
	int ret = 0;
	while (*text != '\0') {
		if(0);
		else if ((ret = get_command(&text, &elem)) != 0);
		else if ((ret = get_register(&text, &elem)) != 0);
		else if ((ret = get_integer(&text, &elem)) != 0);
		else if ((ret = get_label(&text, &elem, dbase)) != 0);
		else if ((ret = get_direct(&text, &elem, dbase)) != 0);
		else if ((ret = get_name(&text, &elem, dbase)) == 0) {
			serrno = SUNKNWN_SYNT;
			fprintf(stderr, "Error: %s: Unknown syntax: %.10s\n",	\
				__FUNCTION__, text);
			return -1;
		}
		if (ret == -1) {
			serrno = SWRONG_SYNT;
			fprintf(stderr, "Error: %s: Wrong syntax: %.10s\n",	\
				__FUNCTION__, text);
			sasm_delete_codelist(dbase->code);
			sasm_delete_labellist(dbase->label);
			return -1;
		}
		while (isspace(*text))
			text++;
	}
	return 0;
}

/** Find token and move *text
 * [return] 0 if token n/f, 1 if found.
 * [note] This func will eat token with spaces if founds it.
 * [note] !isalnum expected after token
*/
int parse_detect_separated(char **text, char *token)
{
	char *tmp = *text;
	while (isspace(*tmp))
		tmp++;
	for (; (*tmp != '\0') && (*token != '\0'); tmp++, token++) {
		if (*tmp != *token)
			return 0;
	}
 	// end of token not reached -> token n/f
	// !isalnum expected after token
	if(*token != '\0' || isalnum(*tmp))
		return 0;
	*text = tmp;
	return 1;
}

/** Find token and move *text
 * [return] 0 if token n/f, 1 if found.
 * [note] This func will eat token with spaces if founds it.
*/
int parse_detect_unsepar(char **text, char *token)
{
	char *tmp = *text;
	while (isspace(*tmp))
		tmp++;
	for (; (*tmp != '\0') && (*token != '\0'); tmp++, token++) {
		if (*tmp != *token)
			return 0;
	}
 	// end of token not reached -> token n/f
	if(*token != '\0')
		return 0;
	*text = tmp;
	return 1;
}

// Calloc-handling paste for get_... functions
#define NEXT_ELEM()                                                     \
do {									\
(*elem)->next = (struct elem_t*) calloc(sizeof(struct elem_t), 1);      \
if((*elem)->next == NULL)						\
        return -1;                                                      \
(*elem) = (*elem)->next;						\
} while(0);

/* Addr. getter
 * [return] adr n/f	0
 * [return] [reg]	1
 * [return] [reg + int] 2
*/
int get_addressing(char **text, struct elem_t **elem)
{
	int retval = 0;
	DUMP();

	if (!parse_detect_unsepar(text, SASM_STR_MEMOP))
		return 0;
	if (!get_register(text, elem))
		return -1;
	retval = 1;

	if (parse_detect_unsepar(text, "+")) {
		if(get_integer(text, elem) == 1)
			retval = 2;
		else
			return -1;
	} else if (parse_detect_unsepar(text, "-")) {
		struct elem_t *tmp = *elem;
		if(get_integer(text, elem) == 1) {
			tmp->val = -(tmp->val);
			retval = 2;
		} else
			return -1;
	}
	if (!parse_detect_unsepar(text, SASM_STR_MEMCL))
		return -1;
	return retval;
}

/** All get_... functions
 * [return] 0 if n/f, 1 if token found, -1 if there are some errors
 * [note] they eat token with spaces and create new empty elem
*/

/// Insert for get_command
int get_push(char **text, struct elem_t **elem)
{
	if(!parse_detect_separated(text, SASM_STR_PUSH))
		return 0;
	struct elem_t *tmp = *elem;
	tmp->type = SASM_TYPE_CMD;
	NEXT_ELEM();
	if(0);
	else if (get_register(text, elem) == 1) {
		tmp->val = SASM_CMD_pushr;
		return 1;
	} else if (get_integer(text, elem) == 1) {
		tmp->val = SASM_CMD_pushd;
		return 1;
	}
	switch(get_addressing(text, elem)) {
	case 0:
		return -1;
	case 1:
		tmp->val = SASM_CMD_pushm;
		(*elem)->type = SASM_TYPE_DW;
		(*elem)->val = 0;
		NEXT_ELEM();
		break;
	case 2:
		tmp->val = SASM_CMD_pushm;
		break;
	}
	return 1;
}

/// Insert for get_command
int get_pop(char **text, struct elem_t **elem)
{
	if(!parse_detect_separated(text, SASM_STR_POP))
		return 0;
	struct elem_t *tmp = *elem;
	tmp->type = SASM_TYPE_CMD;
	NEXT_ELEM();
	if(0);
	else if (get_register(text, elem) == 1) {
		tmp->val = SASM_CMD_popr;
		return 1;
	}
	switch(get_addressing(text, elem)) {
	case 0:
		return -1;
	case 1:
		tmp->val = SASM_CMD_popm;
		(*elem)->type = SASM_TYPE_DW;
		(*elem)->val = 0;
		NEXT_ELEM();
		break;
	case 2:
		tmp->val = SASM_CMD_popm;
		break;
	}
	return 1;
}

int get_command(char **text, struct elem_t **elem)
{
	DUMP();
	int retval = 0;
	if((retval = get_push(text, elem)) != 0)
		return retval;
	if((retval = get_pop(text, elem)) != 0)
		return retval;
	if (0);
#define CMD(name)					\
        else if (parse_detect_separated(text, #name)) {	\
                (*elem)->type = SASM_TYPE_CMD;         	\
                (*elem)->val = SASM_CMD_##name;        	\
        }
#include "../public/sasm_cmds.h"
#undef CMD
	else
		return 0;
	NEXT_ELEM();
	return 1;
}

int get_register(char **text, struct elem_t **elem)
{
	DUMP();
	if (0);
#define REG(name)					\
        else if (parse_detect_separated(text, #name)) { \
                (*elem)->type = SASM_TYPE_REG;         	\
                (*elem)->val = SASM_REG_##name;        	\
        }
#include "../public/sasm_regs.h"
#undef REG
	else
		return 0;
	NEXT_ELEM();
	return 1;
}

/// Process c-like-syntax integer (strtol())
int get_integer(char **text, struct elem_t **elem)
{
	DUMP();
	while (isspace(**text))
		(*text)++;
	char *endptr = NULL;
	errno = 0;
	int32_t val = strtol(*text, &endptr, 0);
	if(*text == endptr) // If there are no integers
		return 0;
	if(errno || isalnum(*endptr))
		return -1;
	(*elem)->type = SASM_TYPE_DW;
	(*elem)->val = val;
	*text = endptr;
	NEXT_ELEM();
	return 1;
}

/// Function for get_label() and get_name()
/// spaces berore named must be removed
int read_name(char *text, char **name, size_t *namlen)
{
	char *tmp = text;
	while (isalnum(*tmp) || *tmp == '_')
		tmp++;
	*namlen = (size_t) (tmp - text);
	if (*namlen == 0)
		return 0;
	*name = (char*) calloc(*namlen + 1, sizeof(char));
	if(*name == NULL)
		return -1;
	memcpy(*name, text, *namlen);
	return 0;
}

/** get_label() and get_name()
 * Every label in text transforms to TYPE_LABEL elem in code-list
 * Every offset value transform to TYPE_OFFS elem in code-list
 * All unknown names are appending to label-list
 * Elem of label-list contains name of label and ptr to TYPE_LABEL in code-list
 * TYPE_OFFS and TYPE_LABEL elems have ptr to elem in label-list
*/

/// Process sasm label. Syntax: ... label_name: ...
int get_label(char **text, struct elem_t **elem, struct sasm_t *sasm)
{
	DUMP();
	while (isspace(**text))
		(*text)++;
	char *name = NULL;
	size_t namlen = 0;
	if (read_name(*text, &name, &namlen) == -1)
		return -1;
	if (namlen == 0)
		return 0;
	if ((*text)[namlen] != ':') {
		free(name);
		return 0;
	}
	struct label_t *ret = sasm_find_label(sasm, name);
	if(ret == NULL) {
		ret = sasm_add_label(sasm, name);
	} else {
		if (ret->ptr != NULL) { // If it's already defined
			fprintf(stderr, "Error: Label redefinition (%s.10)\n", name);
			free(name);
			return -1;	// Error (redefinition)
		}
		free(name);
	}
	ret->ptr = *elem;
	(*elem)->type = SASM_TYPE_LABEL;
	(*elem)->label = ret;
	NEXT_ELEM();
	*text = *text + (namlen + 1);
	return 1;
}

/// Get unknown name. (if it's unlinked it may cause error)
int get_name(char **text, struct elem_t **elem, struct sasm_t *sasm)
{
	DUMP();
	while (isspace(**text))
		(*text)++;
	char *name = NULL;
	size_t namlen = 0;
	if (read_name(*text, &name, &namlen) == -1)
		return -1;
	if (namlen == 0)
		return 0;
	if (!isspace((*text)[namlen])) {
		free(name);
		return 0;
	}
	struct label_t *ret = sasm_find_label(sasm, name);
	if(ret == NULL)
		ret = sasm_add_label(sasm, name);
	else
		free(name); // If it's already defined
	if(elem)
		(*elem)->type = SASM_TYPE_RELOFFS;
	(*elem)->label = ret;
	NEXT_ELEM();
	(*text) += namlen;
	return 1;
}


// Eats "cstr" 
int read_cstr(char **text, char **name, size_t *namlen)
{
	if (!parse_detect_separated(text, "\"")) {
		*namlen = 0;
		*name = NULL;
		return 0;
	}
	char *tmp = *text;
	while (*tmp != '\"' && *tmp != '\0')
		tmp++;
	if (*tmp != '\"') {
		fprintf(stderr, "Error: EOF reached while reading str\n");
		return -1;
	}
	*namlen = (size_t) (tmp - *text);
	if (*namlen == 0)
		return 0;
	*name = (char*) calloc(*namlen + 1, sizeof(char));
	if(*name == NULL)
		return -1;
	memcpy(*name, *text, *namlen);
	*text = tmp + 1;
	return 0;
}


/// Get sasm directive
int get_direct(char **text, struct elem_t **elem, struct sasm_t *sasm)
{
	if (!parse_detect_separated(text, SASM_STR_EXTERN))
		return 0;
	char *name = NULL;
	size_t namlen = 0;
	if (read_cstr(text, &name, &namlen) == -1)
		return -1;
	if (namlen == 0)
		return -1;	// There MUST BE "filename"
	struct label_t *ret = sasm_find_label(sasm, name);
	if(ret == NULL) {
		ret = sasm_add_label(sasm, name);
	} else {
		if (ret->ptr != NULL) { // If it's already defined
			fprintf(stderr, "Error: Label redefinition (%s.10)\n", name);
			free(name);
			return -1;	// Error (redefinition)
		}
		free(name);
	}
	ret->ptr = *elem;
	(*elem)->type = SASM_TYPE_EXTERN;
	(*elem)->label = ret;
	NEXT_ELEM();
	return 1;
}

