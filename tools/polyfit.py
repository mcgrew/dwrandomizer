#!/usr/bin/env python3

# Sample run:
# xargs python3 polyfit.py < curve_data/player_str.txt

import sys
import os

import numpy as np
import matplotlib.pyplot as plt

if __name__ == "__main__":
    if os.path.exists(sys.argv[1]):
        with open(sys.argv[1], 'r') as f:
            args = [x.strip() for x in f.readlines() if x.strip()]
    else:
        args = sys.argv[1:]
    y = np.array(sorted([float(y.strip()) for y in args[1].split(',')]))
    x = np.array(sorted([float(x.strip()) for x in args[2].split(',')]))
    x_range = range(0, int(max(x)+1))
    try:
        coef = np.polyfit(x, y, int(args[0]))
    except TypeError as e:
        print(f"y={len(y)}, x={len(x)}")
        print(str(e))
        sys.exit()
    poly = np.poly1d(coef)
    poly_values2 = [poly(v) for v in x_range]
    plt.plot(x_range, poly_values2, label="New")


    coef = list(reversed(coef))
    plt.grid(True)

    print('Coefficients:')
    print('{')
    print('    MIN,')
    print('    MAX,')
    for i in range(14):
        if len(coef) > i:
            print(f'    {coef[i]},')
        else:
            print(f'    0.0,')
    print('};')


    plt.scatter(x, y, s=10, label="Fit points")
    label = None
    for arg in args[3:]:
        if arg.startswith('#'):
            label = arg[1:].replace('_', ' ')
            continue
        if arg.startswith('^'):
            factors = [float(x) for x in arg[1:].split(',')]
            poly = np.poly1d(list(reversed(factors)))
            val = np.array([poly(y) for y in x_range])
        else:
            val = np.array([float(y) for y in arg.split(',')])
        plt.plot(range(0, len(val)), val, label=label)
        label = None
    plt.legend()
    plt.show()

