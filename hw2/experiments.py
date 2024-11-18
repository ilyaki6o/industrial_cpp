import input.generator as generator
import subprocess
import time
import pickle
import numpy as np
import matplotlib.pyplot as plt


def experiment1():
    print("Experiment 1: finding parameters with running time > 60s")

    for _ in range(10):
        for N in [22000]:
            for K in [50]:
                file_name = f'input/input_ex1_k20_n{N}.csv'
                generator.generator(K, N, 1, 150, file_name)
                st = time.time()
                p = subprocess.run(f"./experiment {file_name}", shell=True, capture_output=True,
                                   encoding='utf8')
                print(f"{K}-{N}: time {time.time()-st:.2f}")

heavy_N = 22000
heavy_k = 50


def experiment2():
    print(f"Experiment 2: comparison of temperature lowering laws, k = {heavy_k}, N = {heavy_N}")
    mean_runs = 3

    for law in ['boltzmann', 'cauchy', 'third']:
        file_name = f'input/input_ex2_k{heavy_k}_n{heavy_N}_{law}.csv'
        generator.generator(heavy_k, heavy_N, 1, 150, file_name)
        loss = 0
        run_time = 0

        for i in range(mean_runs):
            print(f"Run {i}: {law}")
            st = time.time()
            p = subprocess.run(f"./experiment {file_name} {law}", shell=True, capture_output=True,
                               encoding='utf8')

            dur = time.time() - st
            run_time += dur

            print(f"    time: {dur:.2f}")
            print(f"    loss: {p.stdout.split()[0]}")

            loss += int(p.stdout.split()[0])

        print(f"mean time for law {law} over {mean_runs} runs: {run_time / mean_runs:.2f}")
        print(f"mean loss for law {law} over {mean_runs} runs: {loss / mean_runs:.2f}")


def experiment3():
    print(f"Experiment 3: heatmap, k = 5..{heavy_k}, N = 2200..{heavy_N}, law = boltzmann")

    mean_runs = 5

    procs = [5, 10, 15, 20, 25, 30, 35, 40, 45, 50]
    tasks = [2200 * i for i in range(1, 11)]

    res = np.zeros(len(procs) * len(tasks)).reshape(len(procs), len(tasks))
    for i, proc in enumerate(procs):
        for j, task_num in enumerate(tasks):
            run_time = 0
            file_name = f'input/input_ex3_k{proc}_n{task_num}.csv'
            generator.generator(proc, task_num, 1, 150, file_name)

            print(f"Runs for proc_num {proc}, task_num {task_num}")
            for k in range(mean_runs):
                print(f"  Run {k}")
                st = time.time()
                p = subprocess.run(f"./experiment {file_name} boltzmann", shell=True, capture_output=True,
                                   encoding='utf8')
                dur = time.time() - st
                run_time += dur
                print(f"    time: {dur:.2f}")

            mean_run_time = run_time / mean_runs
            res[i, j] = mean_run_time

            print(f"mean time over {mean_runs} runs: {mean_run_time:.2f}")
            print()

    with open('output/heat_map_data.pkl', 'wb') as f:
        pickle.dump(res, f)


def draw_heat_map(in_file, out_file, labels_x, labels_y):
    with open(in_file, 'rb') as f:
        arr = pickle.load(f)
    fig, ax = plt.subplots()
    im = ax.imshow(arr, cmap='inferno', interpolation='nearest')

    ax.set_yticks(np.arange(len(labels_x)), labels=labels_x)
    ax.set_xticks(np.arange(len(labels_y)), labels=labels_y, rotation=90)
    ax.invert_yaxis()

    plt.colorbar(im, shrink=0.55)
    plt.title("Algorithm execution time, s")
    plt.xlabel("Number of tasks")
    plt.ylabel("Number of processor unit")

    plt.savefig(out_file)

procs = [5, 10, 15, 20, 25, 30, 35, 40, 45, 50]
tasks = [2200 * i for i in range(1, 11)]
#draw_heat_map('output/heat_map_data.pkl', 'output/heat_map.png', procs, tasks)

def experiment4_paral():
    print("Experiment 4 parallel: comparison of parallel with consecutive")
    law = "boltzmann"
    n_proc = 4
    time_incr = []
    loss_decr = []

    for N in [2200 * i for i in range(4, 9)]:
        file_name = f'input/input_ex4_k20_n{N}.csv'
        generator.generator(heavy_k, N, 1, 150, file_name)
        print(f"CONSECUTIVE VERSION k={heavy_k}, N={N}, law={law}")
        st = time.time()
        p = subprocess.run(f"./experiment {file_name} {law}", shell=True, capture_output=True,
                           encoding='utf8')
        t_con = time.time() - st
        loss_con = int(p.stdout.split()[0])
        print(f"    time: {t_con:.2f}")
        print(f"    loss: {loss_con}")

        print(f"PARALLEL VERSION k={heavy_k}, N={N}, law={law}, NPROC={n_proc}")
        st = time.time()
        p = subprocess.run(f"./experiment {file_name} {law} {n_proc}", shell=True, capture_output=True,
                           encoding='utf8')
        t_par = time.time() - st
        loss_par = int(p.stdout.split()[0])
        print(f"    time: {t_par:.2f}")
        print(f"    loss: {loss_par}")
        time_incr.append((t_par-t_con)/t_con * 100)
        loss_decr.append((loss_con - loss_par) / loss_con * 100)
        print(f"time increase by: {time_incr[-1]:.1f}%")
        print(f"loss decrease by: {loss_decr[-1]:.1f}%")
        print()

def experiment5_paral():
    print("Experiment 5 parallel: running time dependency graph")

    mean_runs = 5

    law = "boltzmann"
    n_tasks = 15400
    procs = [1, 2, 3, 4, 5, 6, 7, 8]

    file_name = f'input/input_ex5_k20_n{n_tasks}.csv'
    generator.generator(heavy_k, n_tasks, 1, 150, file_name)

    res = []

    for proc in procs:
        print(f"PARALLEL VERSION k={heavy_k}, N={n_tasks}, law={law}")
        mean_t = []
        print(f"Proccesses number: {proc}")
        for i in range(mean_runs):
            print(f"  Run {i}")
            st = time.time()
            p = subprocess.run(f"./experiment {file_name} {law} {proc}", shell=True, capture_output=True,
                               encoding='utf8')
            mean_t.append(time.time() - st)
            print(f"    time: {mean_t[-1]:.2f}")
        res.append(sum(mean_t)/len(mean_t))
        print(f"mean time: {res[-1]:.2f}")

    res = np.array(res)
    with open('output/parallel_times.pkl', 'wb') as f:
        pickle.dump(res, f)


def draw_plot(in_file, out_file, x_values):
    with open(in_file, 'rb') as f:
        arr = pickle.load(f)
    fig, ax = plt.subplots()
    im = ax.plot(x_values, arr)
    ax.grid(True)
    plt.title("Parallel algorithm execution time")
    plt.xlabel("Number of processes")
    plt.ylabel("Execution time")

    plt.savefig(out_file)


# draw_plot('output/parallel_times.pkl', 'output/parallel_plot.png', np.arange(1, 9))

if __name__ == "__main__":
    #experiment5_paral()
    draw_plot('output/parallel_times.pkl', 'output/parallel_plot.png', np.arange(1, 9))
