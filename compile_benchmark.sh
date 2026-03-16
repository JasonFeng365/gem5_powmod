# Compile the test
gcc -DDIM=12 -O3 -ftree-vectorize -msse4.2 -mfpmath=sse -fopt-info-vec -I include benchmark.c util/m5/build/x86/out/libm5.a -o benchmark