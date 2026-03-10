#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gem5/m5ops.h>
// #include <cstdint>
#include <inttypes.h>


#define start_rec m5_reset_stats(0, 0);
#define end_rec m5_dump_stats(0, 0);

struct int32_state {
	int32_t n, k, m;
};



#define range(i, start, end, iter) for (int i=start; i<end; i+=iter)



// int32_t powmod
static inline uint64_t uint64_powmod(uint64_t n, uint64_t k, uint64_t m)
{
	uint64_t res;
	// fprintf(stderr, "Inside start_powmod_int32\n");
	// asm volatile (".byte 0xDA, 0xE0" : : "a"(ptr) : "memory");
	asm volatile (".byte 0xDA, 0xE0" : "=b"(res) : "a"(n), "c"(k), "d"(m) : );
	// fprintf(stderr, "Result of start_powmod: %" PRIu64 "\n", res);
	return res;
}


static inline void save_powmod_int32(const int32_t *ptr)
{
	fprintf(stderr, "Inside save_powmod_int32\n");
	asm volatile (".byte 0xDA, 0xF9" : : "c"(ptr) : "memory");
}


// static inline void m4_loadb(const float *ptr)
// {
// 	asm volatile (".byte 0xDD, 0xCE" : : "S"(ptr) : "memory");
// }


// static inline void m4_storeout(float *ptr)
// {
// 	asm volatile (".byte 0xDD, 0xF2" : : "d"(ptr) : "memory");
// }

inline uint64_t uint64_powmod_manual(uint64_t n, uint64_t k, uint64_t m) {
	uint64_t res = 1;
	uint64_t base = n;

	while (k) {
		if (k&1) res = (res*base) % m;

		base = (base*base) % m;
		k>>=1;
	}

	return res;
}


int main(void)
{
	// const size_t bytes = DIM*DIM * sizeof(float);
	// float *A = (float *)aligned_alloc(64, bytes);

	// n, k, m
	// struct int32_state state = {51, 1186265532, 39999};
	uint64_t res;
	// res = uint64_powmod(2, 10, 1000000007);

	uint64_t n, k, m;
	scanf("%lu %lu %lu", &n, &k, &m);

	start_rec;
	res = uint64_powmod(n, k, m);
	// res = uint64_powmod_manual(n, k, m);
	// res = uint64_powmod_manual(51, 1186265532, 1000000007);
	end_rec;
	fprintf(stderr, "Result = %lu\n", res);

	fprintf(stderr, "Reached end of program\n");
	return 0;
}
