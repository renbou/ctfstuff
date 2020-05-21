// Apache License
// Created by Artem Mikheev in 2020
//

#ifndef LIBC_FUNCS_H
#define LIBC_FUNCS_H

#include <cstdint>

/* Linear congruential.  */
#define	TYPE_0		0
#define	BREAK_0		8
#define	DEG_0		0
#define	SEP_0		0

/* x**7 + x**3 + 1.  */
#define	TYPE_1		1
#define	BREAK_1		32
#define	DEG_1		7
#define	SEP_1		3

/* x**15 + x + 1.  */
#define	TYPE_2		2
#define	BREAK_2		64
#define	DEG_2		15
#define	SEP_2		1

/* x**31 + x**3 + 1.  */
#define	TYPE_3		3
#define	BREAK_3		128
#define	DEG_3		31
#define	SEP_3		3

/* x**63 + x + 1.  */
#define	TYPE_4		4
#define	BREAK_4		256
#define	DEG_4		63
#define	SEP_4		1

#define	MAX_TYPES	5	/* Max number of types above.  */

// libc random_data struct
struct random_data {
	int32_t *fptr;		/* Front pointer.  */
	int32_t *rptr;		/* Rear pointer.  */
	int32_t *state;		/* Array of state values.  */
	int rand_type;		/* Type of random number generator.  */
	int rand_deg;		/* Degree of random number generator.  */
	int rand_sep;		/* Distance between front and rear.  */
	int32_t *end_ptr;		/* Pointer behind state table.  */
};

// elf hashes
uint32_t dl_new_hash (const char *s);
uint32_t _dl_elf_hash (const char *name);

// linux libc random functions
int __srandom(unsigned int seed, struct random_data *buf);

int __random (struct random_data *buf, int32_t *result);

int srandom(unsigned int seed);

int random();

#endif //LINUX_LIBC_FUNCTIONS_H
