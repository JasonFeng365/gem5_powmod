#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gem5/m5ops.h>


#define start_rec m5_reset_stats(0, 0);
#define end_rec m5_dump_stats(0, 0);

struct int32_state {
	int32_t n, k, m;
};



#define range(i, start, end, iter) for (int i=start; i<end; i+=iter)



// int32_t powmod
static inline void start_powmod_int32(const struct int32_state *ptr)
{
	fprintf(stderr, "Inside start_powmod_int32\n");
	asm volatile (".byte 0xDA, 0xE0" : : "a"(ptr) : "memory");
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



int main(void)
{
	// const size_t bytes = DIM*DIM * sizeof(float);
	// float *A = (float *)aligned_alloc(64, bytes);
	
	// n, k, m
	struct int32_state state = {2, 10, 1000000007};
	start_powmod_int32(&state);
	
	int32_t* res = malloc(4);
	save_powmod_int32(res);
	fprintf(stderr, "%d^%d %% %d = %d\n", state.n, state.k, state.m, res[0]);


	
	// start_rec;
	// BENCH_FUNC;
	// end_rec;

	// free(tiledOut);
	fprintf(stderr, "Reached end of program\n");
	return 0;
}