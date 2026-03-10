#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>


// 0 1 2 3 4 5 6 7  ...
// 0 1 1 2 3 5 8 13 ...

#define bRows aCols
#define range(i, start, end, iter) for (int i=start; i<end; i+=iter)

int32_t matmult(uint64_t *A, uint64_t *B, uint64_t *dst, uint64_t mod, int aRows, int aCols, int bCols) {
	memset(dst, 0, (aRows * bCols));

	range(i, 0, aRows, 1) {
		range(j, 0, aCols, 1) {
			range(k, 0, bCols, 1) {
				dst[aRows*i + k] = (dst[aRows*i + k] + A[aRows*i + j] * B[bRows*j + k]) % mod;
			}
		}
	}
}

// inline uint64_t uint64_powmod_manual(uint64_t n, uint64_t k, uint64_t m) {
// 	uint64_t res = 1;
// 	uint64_t base = n;

// 	while (k) {
// 		if (k&1) res = (res*base) % m;

// 		base = (base*base) % m;
// 		k>>=1;
// 	}

// 	return res;
// }

uint64_t uint64_fibmod(uint64_t i, uint64_t m) {
	uint64_t res[2] = {0, 1};
	uint64_t base[4] = {0, 1, 1, 1};

	while (i) {
		if (i&1) {
			uint64_t temp[2];
			matmult(res, base, temp, m, 1, 2, 2);
			memcpy(&res, &temp, 2);
		}

		uint64_t temp[4];
		matmult(base, base, temp, m, 2, 2, 2);
		memcpy(&base, &temp, 4);
		i>>=1;
	}

	return res[0];
}


int main(void)
{
	uint64_t res[10];
	range(i, 0, 10, 1) {
		printf("Starting powmod on %d\n", i);
		res[i] = uint64_fibmod(i, 1000000007);
		printf("%d: %lu\n", i, res[i]);
	}



	return 0;
}
