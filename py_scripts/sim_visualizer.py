import os
import pandas as pd
import matplotlib.pyplot as plt

# ---------------------------------------------------------
# Find CSV from simulation dir in paths.txt
# ---------------------------------------------------------
PATHS_FILE = "paths.txt"
KEY = "SIMULATION"

sim_dir = None
with open(PATHS_FILE, "r") as f:
    for line in f:
        line = line.strip()
        if not line or "=" not in line:
            continue

        k, _, v = line.partition("=")
        if k.strip() == KEY:
            sim_dir = v.strip()
            break

if sim_dir is None:
    raise RuntimeError(f"Key '{KEY}' not found in {PATHS_FILE}")

sim_dir = os.path.abspath(sim_dir)

if not os.path.isdir(sim_dir):
    raise RuntimeError(f"Simulation directory does not exist: {sim_dir}")

csv_path = os.path.join(sim_dir, "results.csv")

if not os.path.isfile(csv_path):
    raise RuntimeError(f"CSV file not found in simulation directory: {csv_path}")

# ---------------------------------------------------------
# Read CSV
# ---------------------------------------------------------
df = pd.read_csv(csv_path)

strategies = [
    ("ALWAYS_LOCAL", 's', "Aina paikallisesti"),
    ("ALWAYS_OFFLOAD", 'o', "Ulkoista aina"),
    ("GREEDY", '^', "Ahne ulkoistaminen"),
    ("LYAPUNOV", 'x', "Lyapunov-optimointi"),
    #("REINFORCEMENT_LEARNING", 'D')
]

# ---------------------------------------------------------
# Plotting
# ---------------------------------------------------------

fig, axes = plt.subplots(3, 1, figsize=(10, 15))
energy_ax, delay_ax, ratio_ax = axes

for strategy, marker_type, label in strategies:
    e_col = f"{strategy}_e_total"
    d_col = f"{strategy}_d_total"
    offload_col = f"{strategy}_offload_count"
    task_col = f"{strategy}_task_total"
    # offloading ratio
    ratio = df[offload_col] / df[task_col]

    sweep_col = df.columns[0]
    energy_ax.plot(df[sweep_col], df[e_col], marker=marker_type, markersize=5, label=label)
    delay_ax.plot(df[sweep_col], df[d_col], marker=marker_type, markersize=5, label=label)
    ratio_ax.plot(df[sweep_col], ratio, marker=marker_type, markersize=5, label=label)

energy_ax.set_title(f"Total Energy vs {sweep_col}")
energy_ax.set_xlabel(sweep_col)
energy_ax.set_ylabel("Total Energy (J)")
energy_ax.legend()

delay_ax.set_title(f"Total Delay vs {sweep_col}")
delay_ax.set_xlabel(sweep_col)
delay_ax.set_ylabel("Total Delay (s)")
delay_ax.legend()

ratio_ax.set_title(f"Offloading Ratio vs {sweep_col}")
ratio_ax.set_xlabel(sweep_col)
ratio_ax.set_ylabel("Offloading Ratio")
ratio_ax.legend()

plt.tight_layout()
plt.show()
