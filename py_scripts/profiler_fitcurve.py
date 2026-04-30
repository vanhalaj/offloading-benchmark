import os
import numpy as np
import matplotlib.pyplot as plt
from keys import get_target

# ---------------------------------------------------------
# Find CSV from profiler dir in paths.txt
# ---------------------------------------------------------
profiler_dir = get_target("PROFILER")

csv_path = os.path.join(profiler_dir, "results.csv")

if not os.path.isfile(csv_path):
    raise RuntimeError(f"CSV file not found in profiler directory: {csv_path}")

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
