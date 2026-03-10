import json
import pandas as pd
import matplotlib.pyplot as plt

data = [json.loads(line) for line in open("results/results.jsonl")]

df = pd.DataFrame(data)


for grid in sorted(df.grid.unique()):
    subset = df[df.grid == grid]
    subset = subset.sort_values("threads")

    plt.plot(
        subset.threads,
        subset.runtime,
        marker="o",
        label=f"{grid}x{grid}"
    )

plt.xlabel("Threads")
plt.ylabel("Runtime (seconds)")
plt.title("Thread Scaling")
plt.legend()
plt.grid(True)
plt.savefig("results/threadScaling.png")
plt.show()


for grid in sorted(df.grid.unique()):
    subset = df[df.grid == grid]
    subset = subset.sort_values("threads")

    baseline = subset[subset.threads == 1].runtime.values[0]
    speedup = baseline / subset.runtime

    plt.plot(
        subset.threads,
        speedup,
        marker="o",
        label=f"{grid}x{grid}"
    )

plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.title("Parallel Speedup")
plt.legend()
plt.grid(True)
plt.savefig("results/Parallel_speedup.png")
plt.show()



for grid in sorted(df.grid.unique()):
    subset = df[df.grid == grid]
    subset = subset.sort_values("threads")

    plt.plot(
        subset.threads,
        subset.paths_per_second,
        marker="o",
        label=f"{grid}x{grid}"
    )

plt.xlabel("Threads")
plt.ylabel("Paths per second")
plt.title("Throughput vs Threads")
plt.legend()
plt.grid(True)
plt.savefig("results/Throughput_threads.png")
plt.show()