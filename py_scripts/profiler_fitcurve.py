import os
import numpy as np
import matplotlib.pyplot as plt

# ---------------------------------------------------------
# Find CSV from profiler dir in paths.txt
# ---------------------------------------------------------
PATHS_FILE = "paths.txt"
KEY = "PROFILER"

profiler_dir = None
with open(PATHS_FILE, "r") as f:
    for line in f:
        line = line.strip()
        if not line or "=" not in line:
            continue

        k, _, v = line.partition("=")
        if k.strip() == KEY:
            profiler_dir = v.strip()
            break

if profiler_dir is None:
    raise RuntimeError(f"Key '{KEY}' not found in {PATHS_FILE}")

profiler_dir = os.path.abspath(profiler_dir)

if not os.path.isdir(profiler_dir):
    raise RuntimeError(f"Profiler directory does not exist: {profiler_dir}")

csv_path = os.path.join(profiler_dir, "results.csv")

if not os.path.isfile(csv_path):
    raise RuntimeError(f"CSV file not found in profiler directory: {csv_path}")

print(f"Using profiler directory: {profiler_dir}")
print(f"Loading CSV: {csv_path}")

# ---------------------------------------------------------
# Load CSV
# ---------------------------------------------------------
data = np.loadtxt(csv_path, delimiter=",", skiprows=1)
n = data[:, 0]
T = data[:, 1]

# ---------------------------------------------------------
# Fitting
# ---------------------------------------------------------
X = np.column_stack([n**3, n**2, n, np.ones_like(n)])
coeffs, _, _, _ = np.linalg.lstsq(X, T, rcond=None)

a, b, c, d = coeffs

print("\nFitted model coefficients:")
print(f"a = {a}")
print(f"b = {b}")
print(f"c = {c}")
print(f"d = {d}")

# ---------------------------------------------------------
# Predict
# ---------------------------------------------------------
def predict(n):
    return a*n**3 + b*n**2 + c*n + d

# ---------------------------------------------------------
# Plotting
# ---------------------------------------------------------
plt.figure(figsize=(10, 6))
n_kb = n * 0.001

# profiled medians
plt.scatter(n_kb, T, color="blue", label="Measured data", s=40)

# fitted curve
x_fit = np.linspace(min(n_kb), max(n_kb), 300)
y_fit = predict(x_fit*1000)
plt.plot(x_fit, y_fit, color="red", linewidth=2, label="Fitted cubic curve")

plt.title("Profiler Runtime vs Input Size")
plt.xlabel("Input size (kilobytes)")
plt.ylabel("Cycles")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()
