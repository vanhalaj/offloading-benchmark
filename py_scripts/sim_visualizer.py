import os
import pandas as pd
import matplotlib.pyplot as plt
from keys import get_target

SHOW_SUB_TITLES = False

# ---------------------------------------------------------
# Find CSV files from simulation dir in paths.txt
# ---------------------------------------------------------
sim_dir = get_target("SIMULATION")

csv_files = [
    f for f in os.listdir(sim_dir)
    if f.endswith(".csv")
] 

if not csv_files:
    raise RuntimeError(f"No CSV files found in simulation directory: {csv_path}")

# ---------------------------------------------------------
# Plotting functions
# ---------------------------------------------------------
strategies = [
    ("ALWAYS_LOCAL", 's', ':', "Aina paikallisesti"),
    ("ALWAYS_OFFLOAD", 'o', ':', "Ulkoista aina"),
    #("OPTIMAL", 's', '-', "Optimaalinen ratkaisu"),
    ("GREEDY", '^', '-', "Ahne ulkoistaminen"),
    ("LYAPUNOV", 'x', '-', "Lyapunov-optimointi"),
    ("REINFORCEMENT_LEARNING", 'D', '-', "Q-vahvistusoppiminen")
]

fig, axes = plt.subplots(3, 1, figsize=(10, 15))
energy_ax, delay_ax, ratio_ax = axes
current_index = 0

def plot_csv(file_name):
    """Plot csv with given file name"""
    energy_ax.clear()
    delay_ax.clear()
    ratio_ax.clear()

    csv_path = os.path.join(sim_dir, file_name)
    df = pd.read_csv(csv_path)
    
    sweep_col = df.columns[0]
    for strategy, marker_type, line_style, label in strategies:
        e_col = f"{strategy}_e_total"
        d_col = f"{strategy}_d_total"
        offload_col = f"{strategy}_offload_count"
        task_col = f"{strategy}_task_total"
        # offloading ratio
        ratio = df[offload_col] / df[task_col]

        energy_ax.plot(df[sweep_col], df[e_col], marker=marker_type, linestyle=line_style, markersize=5, label=label)
        delay_ax.plot(df[sweep_col], df[d_col], marker=marker_type, linestyle=line_style, markersize=5, label=label)
        ratio_ax.plot(df[sweep_col], ratio, marker=marker_type, linestyle=line_style, markersize=5, label=label)

    if SHOW_SUB_TITLES:
        # energy_ax.set_title(f"Total Energy vs {sweep_col}")
        energy_ax.set_title(f"Kokonaisenergia vs {sweep_col}")
        # delay_ax.set_title(f"Total Delay vs {sweep_col}")
        delay_ax.set_title(f"Kokonaisviive vs {sweep_col}")
        # ratio_ax.set_title(f"Offloading Ratio vs {sweep_col}")
        ratio_ax.set_title(f"Ulkoistamisen osuus vs {sweep_col}")

    energy_ax.set_xlabel(sweep_col)
    # energy_ax.set_ylabel("Total Energy (J)")
    energy_ax.set_ylabel("Kokonaisenergia (J)")
    energy_ax.legend()

    delay_ax.set_xlabel(sweep_col)
    # delay_ax.set_ylabel("Total Delay (s)")
    delay_ax.set_ylabel("Kokonaisviive (s)")
    delay_ax.legend()

    ratio_ax.set_xlabel(sweep_col)
    # ratio_ax.set_ylabel("Offloading Ratio")
    ratio_ax.set_ylabel("Ulkoistamisen osuus")
    ratio_ax.legend()

    # no empty space on left and right sides
    for ax in axes:
        ax.margins(x=0)

    fig.canvas.manager.set_window_title(f"Viewing file {file_name} ({current_index+1}/{len(csv_files)})")

    name_without_ext = os.path.splitext(file_name)[0]
    fig.canvas.get_default_filename = lambda: f"{name_without_ext}.png"

    plt.tight_layout()
    plt.show()

def on_key(event):
    """Change between plots"""
    global current_index
    if event.key == "right" or event.key == "pagedown":
        current_index = (current_index + 1) % len(csv_files)
        plot_csv(csv_files[current_index])
    elif event.key == "left" or event.key == "pageup":
        current_index = (current_index - 1) % len(csv_files)
        plot_csv(csv_files[current_index])

# ---------------------------------------------------------
# Show initial plots
# ---------------------------------------------------------
fig.canvas.mpl_connect("key_press_event", on_key)
plot_csv(csv_files[current_index])
plt.show()
