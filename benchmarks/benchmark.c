#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gem5/m5ops.h>


#define start_rec m5_reset_stats(0, 0);
#define end_rec m5_dump_stats(0, 0);



#define range(i, start, end, iter) for (int i=start; i<end; i+=iter)



/**
 * LOADA: Load a 4x4 tile into the A queue.
 * Encoding: opcode=0xDA, ModRM=0xE0 (reg=4, mod=3, rm=0 for RAX)
 */
static inline void m4_loada(const float *ptr)
{
    asm volatile (".byte 0xDA, 0xE0" : : "a"(ptr) : "memory");
}

/**
 * LOADB: Load a 4x4 tile into the B queue.
 * When both A and B queues have tiles, hardware computes Out += A * B.
 * Encoding: opcode=0xDD, ModRM=0xCE (reg=1, mod=3, rm=6 for RSI)
 */
static inline void m4_loadb(const float *ptr)
{
    asm volatile (".byte 0xDD, 0xCE" : : "S"(ptr) : "memory");
}

/**
 * LOADOUT
 * Encoding: opcode=0xDD, ModRM=0xCE (reg=7, mod=3, rm=1 for RCX)
 */
static inline void m4_loadout(const float *ptr)
{
    asm volatile (".byte 0xDA, 0xF9" : : "c"(ptr) : "memory");
}

/**
 * STOREOUT: Store the Out accumulator to memory and clear it.
 * Encoding: opcode=0xDD, ModRM=0xF2 (reg=6, mod=3, rm=2 for RDX)
 */
static inline void m4_storeout(float *ptr)
{
    asm volatile (".byte 0xDD, 0xF2" : : "d"(ptr) : "memory");
}


int main(void)
{
    /* Allocate 64-byte aligned memory for tiles (required by M4).
     * Note: aligned_alloc requires size to be a multiple of alignment. */
    const size_t bytes = DIM*DIM * sizeof(float);
    float *A = (float *)aligned_alloc(64, bytes);
    float *B = (float *)aligned_alloc(64, bytes);
    float *Out = (float *)aligned_alloc(64, bytes);
    float *OutRef = (float *)aligned_alloc(64, bytes);

	float *tiledA = (float *)aligned_alloc(64, bytes);
    float *tiledB = (float *)aligned_alloc(64, bytes);
    float *tiledOut = (float *)aligned_alloc(64, bytes);

    if (!A || !B || !Out || !OutRef || !tiledA || !tiledB || !tiledOut) {
        fprintf(stderr, "aligned_alloc failed\n");
        return 1;
    }


    /* Compute reference result */
    // row_major_untiled_ijk(A, B, OutRef);
	start_rec;
	BENCH_FUNC;
	// tile_major_m4_ijk(tiledA, tiledB, tiledOut);
	end_rec;

    // free(A);
    // free(B);
    // free(Out);
    // free(OutRef);
	// free(tiledA);
	// free(tiledB);
	// free(tiledOut);
	fprintf(stderr, "Reached end of program\n");
    // return errors ? 1 : 0;
	return 0;
}