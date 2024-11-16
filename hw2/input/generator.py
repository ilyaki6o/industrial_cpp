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
