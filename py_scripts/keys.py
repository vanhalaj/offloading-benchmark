import os

def get_target(key: str):
    """
    Read value (directory) of given key from paths.txt
    """
    PATHS_FILE = "paths.txt"

    target_dir = None
    with open(PATHS_FILE, "r") as f:
        for line in f:
            line = line.strip()
            if not line or "=" not in line:
                continue

            k, _, v = line.partition("=")
            if k.strip() == key:
                target_dir = v.strip()
                break

    if target_dir is None:
        raise RuntimeError(f"Key '{key}' not found in {PATHS_FILE}")

    target_dir = os.path.abspath(target_dir)

    if not os.path.isdir(target_dir):
        raise RuntimeError(f"Simulation directory does not exist: {target_dir}")

    print(f"{key} directory: {target_dir}")
    return target_dir