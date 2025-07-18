# env_config.py

import os


def load_env_vars(env_file=".env"):
    result = {}
    if not os.path.exists(env_file):
        return result
    with open(env_file) as f:
        for line in f:
            if "=" in line:
                key, val = line.strip().split("=", 1)
                result[key] = val.strip('"').strip("'")
    return result

env_vars = load_env_vars()
with open("env_flags.ini", "w") as f:
    for key, value in env_vars.items():
        f.write(f"-D{key}={value}\n")