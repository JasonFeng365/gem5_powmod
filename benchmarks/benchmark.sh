GEM5="/root/gem5_powmod"
SRC="benchmark.c"
OUT="benchmark"

echo "Compiling $SRC"
gcc -O3 -ftree-vectorize -msse4.2 -mfpmath=sse -fopt-info-vec \
    -I ${GEM5}/include \
    benchmark.c \
    ${GEM5}/util/m5/build/x86/out/libm5.a \
    -o ${OUT}

RUNS=(
    "intpow"
    #"intpow-manual"
    #"dblpow"
    #"dblpow-manual"
)

K_VALUES=(
# "1"
# "2"
# "3"
# "4"
# "7"
# "8"
# "15"
# "16"
# "31"
# "32"
# "63"
# "64"
# "127"
# "128"
# "255"
# "256"
# "511"
# "512"
# "1023"
# "1024"
# "2047"
# "2048"
# "4095"
# "4096"
# "8191"
# "8192"
# "16383"
# "16384"
# "32767"
# "32768"
# "65535"
# "65536"
# "131071"
# "131072"
# "262143"
# "262144"
# "524287"
# "524288"
# "1048575"
# "1048576"
# "2097151"
# "2097152"
# "4194303"
# "4194304"
# "8388607"
# "8388608"
# "16777215"
# "16777216"
# "33554431"
# "33554432"
# "67108863"
# "67108864"
# "134217727"
# "134217728"
# "268435455"
# "268435456"
# "536870911"
# "536870912"
# "1073741823"
# "1073741824"
# "2147483647"
# "2147483648"
# "4294967295"
# "4294967296"   
"18446744073709551615"
)

for k in "${K_VALUES[@]}"; do
    for run in "${RUNS[@]}"; do
        $GEM5/build/X86/gem5.opt -re \
        --outdir m5out/${run}/${k} \
        $GEM5/configs/deprecated/example/se.py \
        --cpu-type=O3CPU --caches --cmd=./benchmark \
        --options "${run} 7 ${k} 1000000007" \
    done
done

# for run in "${RUNS[@]}"; do
#     $GEM5/build/X86/gem5.opt -re \
#     --outdir m5out/${run}/med \
#     $GEM5/configs/deprecated/example/se.py \
#     --cpu-type=O3CPU --caches --cmd=./benchmark \
#     --options "${run} 7 1023 1000000007"
# done

# for run in "${RUNS[@]}"; do
#     $GEM5/build/X86/gem5.opt -re \
#     --outdir m5out/${run}/large \
#     $GEM5/configs/deprecated/example/se.py \
#     --cpu-type=O3CPU --caches --cmd=./benchmark \
#     --options "${run} 7 4294967295 1000000007"
# done

# for run in "${RUNS[@]}"; do
#     $GEM5/build/X86/gem5.opt -re \
#     --outdir m5out/${run}/dense \
#     $GEM5/configs/deprecated/example/se.py \
#     --cpu-type=O3CPU --caches --cmd=./benchmark \
#     --options "${run} 7 1048575 1000000007"
# done

# for run in "${RUNS[@]}"; do
#     $GEM5/build/X86/gem5.opt -re \
#     --outdir m5out/${run}/sparse \
#     $GEM5/configs/deprecated/example/se.py \
#     --cpu-type=O3CPU --caches --cmd=./benchmark \
#     --options "${run} 7 1048576 1000000007"
# done

echo "All runs complete."


