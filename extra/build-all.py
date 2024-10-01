#!/usr/bin/env python3

import sys
import subprocess
from tabulate import tabulate
from concurrent.futures import ThreadPoolExecutor, as_completed

targets = ["x86", "x64", "armv6m", "armv7m", "armv7emsp", "armv7emdp", "xtensa", "xtensawin"]  # "rv32imc"
configs = ["test"]


# Initialize a dictionary of dictionaries to store results
results = {target: {config: None for config in configs} for target in targets}


# Function to run the make command and return the result
def build(target, config):
    print(f"Building {target} - {config}")
    result = subprocess.run(
        [
            "make",
            "clean",
            "all",
            #"V=1",
            f"ARCH={target}",
            f"BUILD=build-{target}-{config}",
        ],
        capture_output = True,
        text = True,
    )
    if result.returncode == 0:
        return (target, config, "🟢")
    else:
        print(f"==== FAILED {target} - {config} ====")
        print(result.stdout)
        print(result.stderr)
        return (target, config, "🟥")


# Execute builds in parallel
with ThreadPoolExecutor(max_workers=1) as executor:
    # Create a future for each build task
    futures = {
        executor.submit(build, target, config): (target, config)
        for target in targets
        for config in configs
    }

    # Collect the results as they complete
    for future in as_completed(futures):
        target, config, result = future.result()
        results[target][config] = result

# Prepare the data for tabulate
table = [[results[target][config] for target in targets] for config in configs]

# Display the transposed results using tabulate
print(tabulate(table, headers=targets, showindex=configs, tablefmt="simple_outline"))

# Detect and report failure
if any(item != "🟢" for row in table for item in row):
    sys.exit(1)
