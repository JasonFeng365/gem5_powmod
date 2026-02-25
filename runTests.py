import subprocess

matSizes = [1<<i for i in range(2, 8)]
loopOrders = ["ijk", "ikj"]
algorithms = ["row_major_untiled", "row_major_tiled", "tile_major_manual", "tile_major_m4"]

simulateCommand = "./build/X86/gem5.opt -re --outdir m5out/benchmark configs/deprecated/example/se.py --cpu-type=O3CPU --caches --cmd=./benchmark"
for algorithm in algorithms:
	params = "(A, B, OutRef)" if "tile_" not in algorithm else "(tiledA, tiledB, tiledOut)"
	doRowTileConvert = "" if "tile_" not in algorithm else "-DCONVERT_ROW_TILE"
	for loopOrder in loopOrders:
		functionCall = f"{algorithm}_{loopOrder}{params}"
		for matSize in matSizes:
			print(f"Starting {algorithm}_{matSize}_{loopOrder}")
			compileCommand = command_string = f"gcc -DDIM={matSize} -DBENCH_FUNC=\"{functionCall}\" {doRowTileConvert} -O3 -ftree-vectorize -msse4.2 -mfpmath=sse -fopt-info-vec -I include benchmark.c util/m5/build/x86/out/libm5.a -o benchmark"
			subprocess.run(command_string, shell=True)
			subprocess.run(simulateCommand, shell=True)

			subprocess.run(f"cp ./m5out/benchmark/stats.txt ./testbench/{algorithm}_{matSize}.txt", shell=True)
