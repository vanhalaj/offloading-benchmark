import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import numpy as np
from keys import get_target

SHOW_SUB_TITLES = False

AXIS_MAP = {
    "f_local": {
        "scale": lambda x: x / (1000*1000),
        "label": "$f_{local}$ (MHz)"
    },
    "f_off": {
        "scale": lambda x: x / (1000*1000),
        "label": "$f_{off}$ (MHz)"
    },
    "d_latency": {
        "scale": lambda x: x * 1000,
        "label": r"$\overline{d}_{latency}$ (ms)"
    },
    "d_latency (var)": {
        "scale": lambda x: x * 1000,
        "label": "$d_{jitter}$ (ms)"
    },
    "R_up": {
        "scale": lambda x: x / (1000*1000) * 8,
        "label": "$R_{up}$ (Mb/s)"
    },
    "R_down": {
        "scale": lambda x: x / (1000*1000) * 8,
        "label": "$R_{down}$ (Mb/s)"
    },
    "P_load": {
        "scale": lambda x: x,
        "label": "$P_{load}$ (W)"
    },
    "P_idle": {
        "scale": lambda x: x,
        "label": "$P_{idle}$ (W)"
    },
    "P_tx": {
        "scale": lambda x: x,
        "label": "$P_{tx}$ (W)"
    },
    "P_rx": {
        "scale": lambda x: x,
        "label": "$P_{rx}$ (W)"
    },
    "c_T": {
        "scale": lambda x: x / (1000*1000),
        "label": r"$\overline{c}_{T}$ (miljoonaa sykliä)"
    },
    "c_T (var)": {
        "scale": lambda x: x / (1000*1000),
        "label": "variance $c_{T}$ (miljoonaa sykliä)" # TODO
    },
    "L_T": {
        "scale": lambda x: x / 1000,
        "label": "$L_{T}$ (kB)"
    },
    "L_T (var)": {
        "scale": lambda x: x / 1000,
        "label": "variance $L_{T}$ (kB)" # TODO
    },
    "dependency_rate": {
        "scale": lambda x: x,
        "label": "riippuvaisten tehtävien suhteellinen määrä"
    }
}

def map_axis(series, name):
    if name in AXIS_MAP:
        return AXIS_MAP[name]["scale"](series), AXIS_MAP[name]["label"]
    return series, name

# ---------------------------------------------------------
# Find CSV files from simulation dir in paths.txt
# ---------------------------------------------------------
sim_dir = get_target("SIMULATION")

csv_files = [
    f for f in os.listdir(sim_dir)
    if f.endswith(".csv")
] 

if not csv_files:
    raise RuntimeError(f"No CSV files found in simulation directory: {csv_files}")

# ---------------------------------------------------------
# Plotting functions
# ---------------------------------------------------------
# Single sweep plotting
strategies = [
    ("ALWAYS_LOCAL", 's', ':', "Aina paikallisesti"),
    ("ALWAYS_OFFLOAD", 'o', ':', "Ulkoista aina"),
    #("OPTIMAL", 's', '-', "Optimaalinen ratkaisu"),
    ("GREEDY", '^', '-', "Ahne ulkoistaminen"),
    ("LYAPUNOV", 'x', '-', "Lyapunov-optimointi"),
    ("REINFORCEMENT_LEARNING", 'D', '-', "Q-oppiminen")
]

fig, axes = plt.subplots(3, 1, figsize=(10, 15))
current_index = 0

def plot_single(file_name):
    """Plot csv with given file name, with single sweep parameter"""
    csv_path = os.path.join(sim_dir, file_name)
    df = pd.read_csv(csv_path)

    fig.clf()
    axes = fig.subplots(3, 1)
    energy_ax, delay_ax, ratio_ax = axes
    
    sweep_col_name = df.columns[0]
    sweep_col, sweep_label = map_axis(df[sweep_col_name], sweep_col_name)

    for strategy, marker_type, line_style, label in strategies:
        e_col = f"{strategy}_e_total"
        d_col = f"{strategy}_d_total"
        offload_col = f"{strategy}_offload_count"
        task_col = f"{strategy}_task_total"
        # offloading ratio
        ratio = df[offload_col] / df[task_col]

        energy_ax.plot(sweep_col, df[e_col], marker=marker_type, linestyle=line_style, markersize=5, label=label)
        delay_ax.plot(sweep_col, df[d_col], marker=marker_type, linestyle=line_style, markersize=5, label=label)
        ratio_ax.plot(sweep_col, ratio, marker=marker_type, linestyle=line_style, markersize=5, label=label)

    if SHOW_SUB_TITLES:
        # energy_ax.set_title(f"Total Energy vs {sweep_label}")
        energy_ax.set_title(f"Kokonaisenergia vs {sweep_label}")
        # delay_ax.set_title(f"Total Delay vs {sweep_label}")
        delay_ax.set_title(f"Kokonaisviive vs {sweep_label}")
        # ratio_ax.set_title(f"Offloading Ratio vs {sweep_label}")
        ratio_ax.set_title(f"Ulkoistamisen osuus vs {sweep_label}")

    energy_ax.set_xlabel(sweep_label)
    # energy_ax.set_ylabel("Total Energy (J)")
    energy_ax.set_ylabel("Kokonaisenergia (J)")
    #energy_ax.legend()

    delay_ax.set_xlabel(sweep_label)
    # delay_ax.set_ylabel("Total Delay (s)")
    delay_ax.set_ylabel("Kokonaisviive (s)")
    #delay_ax.legend()

    ratio_ax.set_xlabel(sweep_label)
    # ratio_ax.set_ylabel("Offloading Ratio")
    ratio_ax.set_ylabel("Ulkoistamisen osuus")
    #ratio_ax.legend()

    handles, labels = energy_ax.get_legend_handles_labels()
    fig.legend(handles, labels, loc="upper center", ncol=len(strategies)/2+1)

    # no empty space on left and right sides
    for ax in axes:
        ax.margins(x=0)

    plt.tight_layout(rect=[0, 0, 1, 0.92])
    plt.draw()

# Nested (double-sweep) plotting
nested_baseline = "ALWAYS_LOCAL"
nested_strategies = [
    #("ALWAYS_OFFLOAD", "Ulkoista aina"),
    #("OPTIMAL", "Optimaalinen ratkaisu"),
    ("GREEDY", "Ahne ulkoistaminen"),
    ("LYAPUNOV", "Lyapunov-optimointi"),
    ("REINFORCEMENT_LEARNING", "Q-oppiminen")
]

# combine color maps so viridis is used between 0.0...1.0 and after 1 red-black gradient is used
limit_value = 1.1
c_n1 = 512
c_n2 = np.int32((limit_value - 1.0) * c_n1)
colors1 = plt.get_cmap("viridis")(np.linspace(0, 1, c_n1))
colors2 = np.zeros((c_n2, 4))
colors2[:, 0] = np.linspace(1, 0, c_n2)
colors2[:, 3] = 1
custom_cmap = colors.ListedColormap(np.vstack((colors1, colors2)))


def plot_nested(file_name):
    """Plot csv that has 2 parameter columns as a 2d heatmap"""
    csv_path = os.path.join(sim_dir, file_name)
    df = pd.read_csv(csv_path)
    sweep_x = df.columns[0]
    sweep_y = df.columns[1]

    fig.clf()
    strat_axes = fig.subplots(len(nested_strategies), 2) # 3 if offloading ratio also wanted
    #cmap = plt.get_cmap("viridis").copy()
    #cmap.set_over("red")
    norm = colors.Normalize(vmin=0, vmax=limit_value, clip=False)

    for i, (strategy, label) in enumerate(nested_strategies):
        strat_baseline_ratio_e = df[f"{strategy}_e_total"] / df[f"{nested_baseline}_e_total"]
        strat_baseline_ratio_d = df[f"{strategy}_d_total"] / df[f"{nested_baseline}_d_total"]
        offloading_ratio = df[f"{strategy}_offload_count"] / df[f"{strategy}_task_total"]
        
        tmp = df[[sweep_x, sweep_y]].copy()
        tmp["energy_ratio"] = strat_baseline_ratio_e
        tmp["delay_ratio"] = strat_baseline_ratio_d
        #tmp["offload_ratio"] = offloading_ratio

        # Pivot into 2D grids
        e_grid = tmp.pivot(index=sweep_y, columns=sweep_x, values="energy_ratio")
        d_grid = tmp.pivot(index=sweep_y, columns=sweep_x, values="delay_ratio")
        #r_grid = tmp.pivot(index=sweep_y, columns=sweep_x, values="offload_ratio")
        
        #e_ax, d_ax, r_ax = strat_axes[i]
        e_ax, d_ax = strat_axes[i]

        im1 = e_ax.imshow(e_grid, aspect='auto', origin='lower', cmap=custom_cmap, norm=norm)
        im2 = d_ax.imshow(d_grid, aspect='auto', origin='lower', cmap=custom_cmap, norm=norm)
        #im3 = r_ax.imshow(r_grid, aspect='auto', origin='lower', cmap=cmap, norm=norm)

        e_ax.set_title(f"{label}, energian suhdeluku")
        d_ax.set_title(f"{label}, viiveen suhdeluku")
        #r_ax.set_title(f"{label} - ulkoistamisen osuus")

        for (ax, grid) in ((e_ax, e_grid), (d_ax, d_grid)): # (r_ax, r_grid)
            x_vals, x_label = map_axis(grid.columns, sweep_x)
            y_vals, y_label = map_axis(grid.index, sweep_y)
            
            tick_step_x = max(1, len(x_vals) // 3)
            tick_step_y = max(1, len(y_vals) // 3)
            
            ticks_x = list(range(0, len(x_vals), tick_step_x))
            ax.set_xticks(ticks_x)
            ax.set_xticklabels(x_vals[ticks_x])
            ax.set_xlabel(x_label)

            ticks_y = list(range(0, len(y_vals), tick_step_y))
            ax.set_yticks(ticks_y)
            ax.set_yticklabels(y_vals[ticks_y])
            ax.set_ylabel(y_label)

        fig.colorbar(im1, ax=e_ax)
        fig.colorbar(im2, ax=d_ax)
        #fig.colorbar(im3, ax=r_ax)

    plt.tight_layout()
    plt.draw()

def plot_csv(file_name: str):
    if file_name.startswith("nested_"):
        plot_nested(file_name)
    else:
        plot_single(file_name)

    if fig.canvas.manager:
        fig.canvas.manager.set_window_title(f"Viewing file {file_name} ({current_index+1}/{len(csv_files)})")
    name_without_ext = os.path.splitext(file_name)[0]
    fig.canvas.get_default_filename = lambda: f"{name_without_ext}.png"

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
