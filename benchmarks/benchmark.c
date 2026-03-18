#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gem5/m5ops.h>

#define ITERATIONS 1000
#define start_rec m5_reset_stats(0, 0);
#define end_rec m5_dump_stats(0, 0);

/**
 * UINT64_POWMOD: Uses the square-and-multiply algorithm (n^k mod m) for modular exponentiation  
 * Encoding: opcode=0xDA, ModRM=0xE0 (reg=4, mod=3, rm=0 for RAX)
 */
static inline uint64_t uint64_powmod(uint64_t n, uint64_t k, uint64_t m)
{
	uint64_t res;
	asm volatile (".byte 0xDA, 0xE0" : "=b"(res) : "a"(n), "c"(k), "d"(m) : );
	return res;
}

/**
 * DOUBLE_POW: Floating-point exponentiation (n^k)
 * Encoding: opcode=0xDA, ModRM=0xF9 (reg=7, mod=3, rm=1 for RAX)
 */
static inline double double_pow(double n, uint64_t k)
{
	double res;
	asm volatile (".byte 0xDA, 0xF9" : "=b"(res) : "a"(n), "c"(k) : );
	return res;
}

/**
 * UINT64_FIBMOD: Computes i-th Fibonacci number mod m (F(i) mod m)
 * Encoding: opcode=0xDA, ModRM=0xCE (reg=1, mod=3, rm=6 for RAX)
 */
static inline uint64_t uint64_fibmod(uint64_t i, uint64_t m)
{
	uint64_t res;
	asm volatile (".byte 0xDD, 0xCE" : "=b"(res) : "a"(i), "c"(m) : );
	return res;
}


static inline uint64_t uint64_powmod_custom(uint64_t n, uint64_t k, uint64_t m) {
	uint64_t res = 1;
	uint64_t base = n;

	while (k) {
		if (k&1) res = uint64_powmod(res,base, m);

		base = uint64_powmod(base, base, m);
		k>>=1;
	}

	return res;
}

/**
 * Manual Implementations of instructions
 */
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

inline double double_pow_manual(double n, uint64_t k) {
	double res = 1;
	double base = n;

	while (k) {
		if (k&1) res*=base;
		base *= base;
		k>>=1;
	}

	return res;
}

void matmult_2_2_2(uint64_t *A, uint64_t *B, uint64_t *dst, uint64_t mod) {
	memset(dst, 0, 4*sizeof(uint64_t));

	#pragma unroll
	for (int i = 0; i < 2; i++) {
		#pragma unroll
		for (int j = 0; j < 2; j++) {
			#pragma unroll
			for (int k = 0; k < 2; k++) {
				dst[2*i + k] = (dst[2*i + k] + A[2*i + j] * B[2*j + k]) % mod;
			}
		}
	}
}

void matmult_1_2_2(uint64_t *A, uint64_t *B, uint64_t *dst, uint64_t mod) {
	memset(dst, 0, 2*sizeof(uint64_t));

	#pragma unroll
	for (int j = 0; j < 2; j++) {
		#pragma unroll
		for (int k = 0; k < 2; j++) {
			dst[k] = (dst[k] + A[j] * B[2*j + k]) % mod;
		}
	}
}

inline uint64_t uint64_fibmod_manual(uint64_t i, uint64_t m) {
	uint64_t res[2] = {0, 1};
	uint64_t base[4] = {0, 1, 1, 1};

	uint64_t temp2[2];
	uint64_t temp4[4];

	while (i) {
		if (i&1) {
			matmult_1_2_2(res, base, temp2, m);
			memcpy(&res, &temp2, 2*sizeof(uint64_t));
		}

		matmult_2_2_2(base, base, temp4, m);
		memcpy(&base, &temp4, 4*sizeof(uint64_t));
		i>>=1;
	}

	return res[0];
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <benchmark> n k m\n", argv[0]);
        return 1;
    }
    const char* bench_sel = argv[1];
	
	// To avoid optimization
	volatile uint64_t result_u64 = 0;
	volatile double result_dbl = 0.0;

	printf("test = %s", bench_sel);
	printf("arg1 = %s", argv[2]);
	printf("arg2 = %s", argv[3]);
	printf("arg3 = %s", argv[4]);

    if (strcmp(bench_sel, "intpow") == 0) {
        uint64_t n = strtoull(argv[2], NULL, 0);
        uint64_t k = strtoull(argv[3], NULL, 0);
        uint64_t m = strtoull(argv[4], NULL, 0);

        start_rec;
		for (int i = 0; i < ITERATIONS; ++i) {
			//result_u64 ^= uint64_powmod(n,k,m);
			result_u64 ^=uint64_powmod_custom(n,k,m);
		}
        end_rec;

		printf("result = %lu", result_u64);
    }
    else if (strcmp(bench_sel, "intpow-manual") == 0) {
        uint64_t n = strtoull(argv[2], NULL, 0);
        uint64_t k = strtoull(argv[3], NULL, 0);
        uint64_t m = strtoull(argv[4], NULL, 0);

        start_rec;
		for (int i = 0; i < ITERATIONS; ++i) {
        	result_u64 ^= uint64_powmod_manual(n,k,m);
		}
        end_rec;

		printf("result = %lu", result_u64);
    }
    else if (strcmp(bench_sel, "dblpow") == 0) {
        double n = strtod(argv[2], NULL);
        uint64_t k = strtoull(argv[3], NULL, 0);
        start_rec;
		for (int i = 0; i < ITERATIONS; ++i) {
        	double_pow(n,k);
		}
        end_rec;
    }
    else if (strcmp(bench_sel, "dblpow-manual") == 0) {
        double n = strtod(argv[2], NULL);
        uint64_t k = strtoull(argv[3], NULL, 0);
        start_rec;
		for (int i = 0; i < ITERATIONS; ++i) {
        	double_pow_manual(n,k);
		}
        end_rec;
    }
    else if (strcmp(bench_sel, "fibmod") == 0) {
        double i = strtoull(argv[2], NULL, 0);
        uint64_t m = strtoull(argv[3], NULL, 0);
        start_rec;
		for (int i = 0; i < ITERATIONS; ++i) {
        	uint64_fibmod(i,m);
		}
        end_rec;
    }
    else if (strcmp(bench_sel, "fibmod-manual") == 0) {
        double i = strtoull(argv[2], NULL, 0);
        uint64_t m = strtoull(argv[3], NULL, 0);
        start_rec;
		for (int i = 0; i < ITERATIONS; ++i) {
        	uint64_fibmod_manual(i,m);
		}
        end_rec;
    }

	return 0;
}