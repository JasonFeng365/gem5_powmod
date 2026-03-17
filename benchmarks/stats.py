import os
import csv

BASE_DIR = "./m5out"
OUTPUT_CSV = "results.csv"

def extract_ticks(stats_path):
    with open(stats_path, "r") as f:
        for line in f:
            if line.startswith("simTicks"):
                parts = line.split()
                if len(parts) >= 2:
                    return int(parts[1])
    return None

rows = []

for measurement in os.listdir(BASE_DIR):
    measurement_path = os.path.join(BASE_DIR, measurement)
    if not os.path.isdir(measurement_path):
        continue

    for number in os.listdir(measurement_path):
        number_path = os.path.join(measurement_path, number)
        stats_file = os.path.join(number_path, "stats.txt")

        if not os.path.isfile(stats_file):
            continue

        ticks = extract_ticks(stats_file)
        if ticks is None:
            continue

        rows.append({
            "measurement": measurement,
            "number": number,
            "ticks": ticks
        })

with open(OUTPUT_CSV, "w", newline="") as csvfile:
    writer = csv.DictWriter(csvfile, fieldnames=["measurement", "number", "ticks"])
    writer.writeheader()
    writer.writerows(rows)

print(f"Wrote {len(rows)} rows to {OUTPUT_CSV}")