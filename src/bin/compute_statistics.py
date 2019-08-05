#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""@package real_time_tools

@file compute_statistics.py
@author Maximilien Naveau (maximilien.naveau@gmail.com)
@copyright Copyright (c) 2019, New York University and Max Planck Gesellschaft.
@license License BSD-3 clause
@date 2019-05-06

@brief This program computes statistics from files in a user given folder.
       The files must possess at least 2 columns. The statistics are going 
       to be computed from the econd one. The typical use is to give this 
       executable a folder path and it will check all files with an 
       extension \".dat\" and perform the statistics
"""

import sys
import argparse
import numpy as np
from os import listdir
from os.path import isfile, join
from time import localtime, strftime

def _get_all_dat_files(folder):
    return [f for f in listdir(folder)
            if (isfile(join(folder, f)) and len(f) >= 4 and '.dat' in f[-4:])]


def _get_data_from_file(in_file_name):
    data=[]
    with open(in_file_name, 'rb') as in_file:
        for row in in_file:
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
        in_dir = args.in_dir
    else:
        in_dir = "."

    files = _get_all_dat_files(in_dir)
    output = ""
    for in_file in files:
        # read the file
        data = _get_data_from_file(in_file)
        # compute the statistics
        _min, _max, _mean, _std_dev = _compute_statitics(data)
        # prepare the output
        output += (
                "The parsed file is: " + in_file + "\n"
                "The computed statistics are the following:\n"
                "    - min: " + str(_min)   + "\n"
                "    - max: " + str(_max)   + "\n"
                "    - mean: " + str(_mean) + "\n"
                "    - standard deviation: " + str(_std_dev)) + "\n\n"

    out_file_name = join(in_dir, "statistics_results_" +
                    strftime("%Y_%m_%d_%H_%M_%S", localtime()) + ".txt")

    print(output)
    with open(out_file_name, 'w') as out_file:
        out_file.write(output)

if __name__ == "__main__":
    main(sys.argv[1:])
    sys.exit(0)

