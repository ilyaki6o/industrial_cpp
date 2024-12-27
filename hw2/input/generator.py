import random
import csv

seed = 42


def generator(k, n, lim1, lim2, file_name):
    with open(file_name, 'w') as csv_file:
        rng = random.Random(seed)
        writer = csv.writer(csv_file)
        data = [k]
        data += [rng.randrange(lim1, lim2+1) for _ in range(n)]
        writer.writerow(data)

if __name__ == "__main__":
    generator(50, 11000, 1, 150, "input/res_11000_50.csv")
