import matplotlib.pyplot as plt

SERIES = 10
RUNS_IN_SERIES = 10

times = [0 for _ in range(RUNS_IN_SERIES)]
best = [0 for _ in range(RUNS_IN_SERIES)]
diff = [0 for _ in range(RUNS_IN_SERIES)]


with open("result_half/data.txt", "r") as f:
    for i in range(SERIES):
        line = f.readline()
        i, data = line.split(": ")
        i = int(i)
        min_score, max_score, time = map(int, data.split(", "))
        times[i] = time // 1000
        best[i] = min_score
        diff[i] = max_score - min_score


plt.plot(diff)
plt.xticks([i for i in range(RUNS_IN_SERIES)])
plt.ylabel("Difference between worst and best solution")
plt.xlabel("i value")
plt.grid(True)
plt.show()

plt.plot(best)
plt.xticks([i for i in range(RUNS_IN_SERIES)])
plt.ylabel("Best metric value")
plt.xlabel("i value")
plt.grid(True)
plt.show()

plt.plot(times)
plt.xticks([i for i in range(RUNS_IN_SERIES)])
plt.ylabel("Execution time, s")
plt.xlabel("i value")
plt.grid(True)
plt.show()
