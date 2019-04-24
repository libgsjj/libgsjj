#!/usr/bin/env python3

import numpy as np

with open("random.data", "w") as f:
    for size in range(1, 11):
        random = np.loadtxt("random-" + str(size))
        means = np.mean(random, axis=0)
        medians = np.median(random, axis=0)
        percentages = np.sum(random, axis=0) / len(random) * 100

        f.write(str(size) + "\t")
        for i in range(0, 8):
            f.write(str(means[i*2]) + "\t" + str(medians[i*2]) + "\t" + str(percentages[i*2+1]) + "\t")
        f.write("\n")

methods = ['"Biermann et Feldman"', '"Neider et Jansen"', '"Unaire (CNF)"', '"Binaire (CNF)"', '"Heule et Verwer (CNF)"', '"Unaire (Non-CNF)"', '"Binaire (Non-CNF)"', '"Heule et Verwer (Non-CNF)"']

with open("random_success.data", "w") as f:
    for i in range(len(methods)):
        f.write(methods[i] + "\t" + str(100 - percentages[i*2+1]) + "\n")
