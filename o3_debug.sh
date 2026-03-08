# Compile the test
gcc -DDIM=12 -O3 -ftree-vectorize -msse4.2 -mfpmath=sse -fopt-info-vec -I include benchmark.c util/m5/build/x86/out/libm5.a -o benchmark 

# Run in gem5 with O3CPU
./build/X86/gem5.opt -re --debug-flags=PowmodAccel,Exec --debug-file=debug.txt --outdir m5out/benchmark configs/deprecated/example/se.py --cpu-type=O3CPU --caches --cmd=./benchmark
# ./build/X86/gem5.opt -re --debug-flags=PowmodAccel,Exec --debug-file=debug.txt --outdir m5out/benchmark configs/deprecated/example/se.py --cpu-type=TimingSimpleCPU --caches --cmd=./benchmark