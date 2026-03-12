#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>


// 0 1 2 3 4 5 6 7  ...
// 0 1 1 2 3 5 8 13 ...

#define bRows aCols
#define range(i, start, end, iter) for (int i=start; i<end; i+=iter)

inline void matmult(uint64_t *A, uint64_t *B, uint64_t *dst, uint64_t mod, int aRows, int aCols, int bCols) {
	memset(dst, 0, (aRows * bCols)*sizeof(uint64_t));

	range(i, 0, aRows, 1) {
		range(j, 0, aCols, 1) {
			range(k, 0, bCols, 1) {
				dst[aRows*i + k] = (dst[aRows*i + k] + A[aRows*i + j] * B[bRows*j + k]) % mod;
			}
		}
	}
}

inline void matmult_2_2_2(uint64_t *A, uint64_t *B, uint64_t *dst, uint64_t mod) {
	memset(dst, 0, 4*sizeof(uint64_t));

	#pragma unroll
	range(i, 0, 2, 1) {
		#pragma unroll
		range(j, 0, 2, 1) {
			#pragma unroll
			range(k, 0, 2, 1) {
				dst[2*i + k] = (dst[2*i + k] + A[2*i + j] * B[2*j + k]) % mod;
			}
		}
	}
}

inline void matmult_1_2_2(uint64_t *A, uint64_t *B, uint64_t *dst, uint64_t mod) {
	memset(dst, 0, 2*sizeof(uint64_t));

	#pragma unroll
	range(j, 0, 2, 1) {
		#pragma unroll
		range(k, 0, 2, 1) {
			dst[k] = (dst[k] + A[j] * B[2*j + k]) % mod;
		}
	}
}

inline uint64_t uint64_fibmod(uint64_t i, uint64_t m) {
	uint64_t res[2] = {0, 1};
	uint64_t base[4] = {0, 1, 1, 1};

	while (i) {
		printf("[[%lu, %lu], [%lu, %lu]]\n", base[0], base[1], base[2], base[3]);
		if (i&1) {
			uint64_t temp[2];
			// matmult(res, base, temp, m, 1, 2, 2);
			matmult_1_2_2(res, base, temp, m);
			memcpy(&res, &temp, 2*sizeof(uint64_t));
		}

		uint64_t temp[4];
		// matmult(base, base, temp, m, 2, 2, 2);
		matmult_2_2_2(base, base, temp, m);
		memcpy(&base, &temp, 4*sizeof(uint64_t));
		i>>=1;
	}

	return res[0];
}


int main(void)
{
	// printf("Hello world!\n");
	// fflush(stdout);
	uint64_t res[10];
	range(i, 0, 10, 1) {
		printf("Starting powmod on %d\n", i);
		fflush(stdout);
		res[i] = uint64_fibmod(i, 1000000007);
		printf("%d: %lu\n", i, res[i]);
		fflush(stdout);
	}



	return 0;
}
