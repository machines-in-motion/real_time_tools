#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import argparse
import numpy as np
from os import listdir
from os.path import isfile, join, splitext

def _get_all_dat_files(folder):
    return [f for f in listdir(folder)
            if (isfile(join(folder, f)) and len(f) >= 4 and '.dat' in f[-4:])]


def _get_data_from_file(file):
    print("Parsing: " + file)
    data=[]
    with open(file, 'rb') as file:
        for row in file:
            txt_row = row.split()
            float_row = [float(i) for i in txt_row]
            data.append(float_row)
    return np.matrix(data)[:,1:]


def _compute_statitics(data):
    _min = []
    _max = []
    _mean = []
    _std_dev = []
    for i in range(data.shape[1]):
        _min    .append(np.min(data[:, i]))
        _max    .append(np.max(data[:, i]))
        _mean   .append(np.mean(data[:, i]))
        _std_dev.append(np.std(data[:, i]))
    return _min, _max, _mean, _std_dev


def main(sys_args):
    # Manages arguments
    description_str = (
        "This program computes statistics from files in a user given folder."
        "The files must possess at least 2 columns. The statistics are going "
        "to be computed from the econd one. The typical use is to give this "
        "executable a folder path and it will check all files with an "
        "extension \".dat\" and perform the statistics")

    parser = argparse.ArgumentParser(description=description_str)
    parser.add_argument('--folder', metavar='in_dir', type=str,
                        help='folder where to look for the *.dat files')
    args = parser.parse_args(sys_args)

    # get files path
    if 'in_dir' in args:
        files = _get_all_dat_files(args.in_dir)
    else:
        files = _get_all_dat_files(".")

    output = ""
    for file in files:
        # read the file
        data = _get_data_from_file(file)
        # compute the statistics
        _min, _max, _mean, _std_dev = _compute_statitics(data)
        # prepare the output
        output += (
                "The parsed file is: " + file + "\n"
                "The computed statistics are the following:\n"
                "    - min: " + str(_min)   + "\n"
                "    - max: " + str(_max)   + "\n"
                "    - mean: " + str(_mean) + "\n"
                "    - standard deviation: " + str(_std_dev)) + "\n"
    print(output)

if __name__ == "__main__":
    main(sys.argv[1:])
    sys.exit(0)

