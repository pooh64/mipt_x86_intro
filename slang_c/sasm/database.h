#ifndef _SASM_DATABASE_
#define _SASM_DATABASE_
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

/// Code elem struct
struct elem_t {
	int type;
	/// Some value (cmd/integer/reg etc)
	int32_t val;
	/// Points to some label if it's TYPE_LABEL or TYPE_RELOFFS
	struct label_t *label;
	struct elem_t *next;
};

/// Struct for label-list
struct label_t {
	char *name;
	/// Redefinition protection
	struct elem_t *ptr;
	int32_t offs;
	struct label_t *next;
};

/// Sasm shell
struct sasm_t {
	struct elem_t *code;
	struct label_t *label;
};

/// Useful database functions
struct label_t *sasm_find_label(struct sasm_t *dbase, char *str);
struct label_t *sasm_add_label(struct sasm_t *dbase, char *str);
int sasm_delete_codelist(struct elem_t *elem);
int sasm_delete_labellist(struct label_t *label);
int sasm_dump_elem(struct elem_t *elem);
int sasm_dump_codelist(struct elem_t *elem);
int sasm_verify_codelist(struct elem_t *elem);
int sasm_verify_labellist(struct label_t *elem);

int sasm_parse(struct sasm_t *dbase, char *text);

/// x64
int sasm_calc_labels_x64(struct sasm_t *dbase, size_t *size);
int sasm_gen_x64(struct sasm_t *dbase, unsigned char *buf);
int sasm_asm_x64(struct sasm_t *dbase, char **buf, size_t *size);

/*	__NOTES__
  TYPE_NULL - term. elem of list
*/

#endif // _SASM_DATABASE_
