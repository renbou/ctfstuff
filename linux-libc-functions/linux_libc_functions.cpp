#include <cstdint>

uint32_t dl_new_hash (const char *s) {
	uint_fast32_t h = 5381;
	for (unsigned char c = *s; c != '\0'; c = *++s)
		h = h * 33 + c;
	return h & 0xffffffff;
}

uint32_t _dl_elf_hash (const char *name) {
	unsigned long h = 0, g;

	while (*name)
	{
		h = (h << 4) + *name++;
		if (g = h & 0xf0000000)
			h ^= g >> 24;
		h &= ~g;
	}
	return h;
}

/* random.h */

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

struct random_data {
	int32_t *fptr;		/* Front pointer.  */
	int32_t *rptr;		/* Rear pointer.  */
	int32_t *state;		/* Array of state values.  */
	int rand_type;		/* Type of random number generator.  */
	int rand_deg;		/* Degree of random number generator.  */
	int rand_sep;		/* Distance between front and rear.  */
	int32_t *end_ptr;		/* Pointer behind state table.  */
};

static int32_t randtbl[DEG_3 + 1] = {
		TYPE_3,
		-1726662223, 379960547, 1735697613, 1040273694, 1313901226,
		1627687941, -179304937, -2073333483, 1780058412, -1989503057,
		-615974602, 344556628, 939512070, -1249116260, 1507946756,
		-812545463, 154635395, 1388815473, -1926676823, 525320961,
		-1009028674, 968117788, -123449607, 1284210865, 435012392,
		-2017506339, -911064859, -370259173, 1132637927, 1398500161,
		-205601318,
		};

static random_data unsafe_state = {
/* FPTR and RPTR are two pointers into the state info, a front and a rear
   pointer.  These two pointers are always rand_sep places apart, as they
   cycle through the state information.  (Yes, this does mean we could get
   away with just one pointer, but the code for random is more efficient
   this way).  The pointers are left positioned as they would be from the call:
	initstate(1, randtbl, 128);
   (The position of the rear pointer, rptr, is really 0 (as explained above
   in the initialization of randtbl) because the state table pointer is set
   to point to randtbl[1] (as explained below).)  */

		.fptr = &randtbl[SEP_3 + 1],
		.rptr = &randtbl[1],

/* The following things are the pointer to the state information table,
   the type of the current generator, the degree of the current polynomial
   being used, and the separation between the two pointers.
   Note that for efficiency of random, we remember the first location of
   the state information, not the zeroth.  Hence it is valid to access
   state[-1], which is used to store the type of the R.N.G.
   Also, we remember the last location, since this is more efficient than
   indexing every time to find the address of the last element to see if
   the front and rear pointers have wrapped.  */

		.state = &randtbl[1],

		.rand_type = TYPE_3,
		.rand_deg = DEG_3,
		.rand_sep = SEP_3,

		.end_ptr = &randtbl[sizeof (randtbl) / sizeof (randtbl[0])]
};

int __srandom(unsigned int seed, struct random_data *buf);
int __random (struct random_data *buf, int32_t *result);
int srand(unsigned int seed);
int rand();

int __srandom(unsigned int seed, struct random_data *buf) {
	int type;
	int32_t *state;
	long int i;
	int32_t word;
	int32_t *dst;
	int kc;

	if (buf == nullptr)
		goto fail;
	type = buf->rand_type;
	if ((unsigned int) type >= MAX_TYPES)
		goto fail;
	state = buf->state;
	/* We must make sure the seed is not 0.  Take arbitrarily 1 in this case.  */
	if (seed == 0)
		seed = 1;
	state[0] = seed;
	if (type == TYPE_0)
		goto done;

	dst = state;
	word = seed;
	kc = buf->rand_deg;
	for (i = 1; i < kc; ++i) {
		/* This does:
		 state[i] = (16807 * state[i - 1]) % 2147483647;
	   but avoids overflowing 31 bits.  */
		long int hi = word / 127773;
		long int lo = word % 127773;
		word = 16807 * lo - 2836 * hi;
		if (word < 0)
			word += 2147483647;
		*++dst = word;
	}

	buf->fptr = &state[buf->rand_sep];
	buf->rptr = &state[0];
	kc *= 10;
	while (--kc >= 0) {
		int32_t discard;
		(void) __random (buf, &discard);
	}

	done:
	return 0;

	fail:
	return -1;
}

int __random (struct random_data *buf, int32_t *result) {
	int32_t *state;

	if (buf == nullptr || result == nullptr)
		return -1;

	state = buf->state;

	if (buf->rand_type == TYPE_0) {
		int32_t val = ((state[0] * 1103515245U) + 12345U) & 0x7fffffff;
		state[0] = val;
		*result = val;
	} else {
		int32_t *fptr = buf->fptr;
		int32_t *rptr = buf->rptr;
		int32_t *end_ptr = buf->end_ptr;
		uint32_t val;

		val = *fptr += (uint32_t) *rptr;
		/* Chucking least random bit.  */
		*result = val >> 1;
		++fptr;
		if (fptr >= end_ptr)
		{
			fptr = state;
			++rptr;
		}
		else
		{
			++rptr;
			if (rptr >= end_ptr)
				rptr = state;
		}
		buf->fptr = fptr;
		buf->rptr = rptr;
	}
	return 0;
}

int srand(unsigned int seed) {
	return __srandom(seed, &unsafe_state);
}

int rand() {
	int result;
	__random(&unsafe_state, &result);
	return result;
}

//#include <cstdio>
//using namespace std;
//
//int main() {
//	srand(10);
//	printf("value: %d\n", rand());
//	return 0;
//}