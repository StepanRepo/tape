#! /bin/python3

import numpy as np
import os

n = 1000

os.system("cp ../main .")

r = np.random.randint(0, 1000, 1000)
np.savetxt("input.dat", r, fmt="%20i")

os.system("../main -c default.cfg")
r_sorted = np.loadtxt("out.dat")

if (np.all(np.sort(r) == r_sorted)):
    print(f"Passed n = {n}")
else:
    print(f"Not passed n = {n}")


