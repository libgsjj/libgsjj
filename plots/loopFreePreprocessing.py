#!/usr/bin/env python3

import numpy as np

# We will keep track of the number of fails a method has
total_fails = np.zeros(shape=(45,8))
# The number of observations we have
number_files = 5 * 9 * 11

with open("loop_free_success.data", "w") as f:
    for size in range(4, 15):
        loop_free = np.loadtxt("loop-free-{size:0>2}".format(size=size))
        # We keep the "fails" columns
        fails = loop_free[:,1::2]
        total_fails += fails
        # The percentage of successes for this file
        success = 100 - np.sum(fails, axis = 0) * 100. / len(fails)
        f.write("{}\t".format(size))
        for s in success:
            f.write("{}\t".format(s))
        f.write("\n")

total_fails = np.asarray(total_fails)
total_succes = 100 - np.sum(total_fails, axis = 0) * 100. / number_files

methods = ['"Biermann et Feldman"', '"Neider et Jansen"', '"Unaire (CNF)"', '"Binaire (CNF)"', '"Heule et Verwer (CNF)"', '"Unaire (Non-CNF)"', '"Binaire (Non-CNF)"', '"Heule et Verwer (Non-CNF)"']

with open("loop_free_success_total.data", "w") as f:
    for i in range(len(total_succes)):
        f.write("{}\t{}\n".format(methods[i], total_succes[i]))