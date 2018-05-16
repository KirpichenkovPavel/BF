#! /usr/bin/python
from numpy import mean
from math import sqrt

# p=0.95 N=120
STUDENT_COEFF = 1.98

def print_average(title, file_name):
    with open(file_name, "r") as f:
        results = f.readlines()
        results = [float(r) for r in results]
        avg = mean(results)
        dev_sqrs = [abs(r-avg)**2 for r in results]
        dev_est = sqrt(sum(dev_sqrs)/(len(dev_sqrs) + 1)) * STUDENT_COEFF
        print("{} {} +- {}".format(title, avg, dev_est))


print_average("Results for 1 process:", "results/results-1.txt")
print_average("Results for 2 processes:", "results/results-2.txt")
print_average("Results for 4 processes:", "results/results-4.txt")
print_average("Results for 16 processes:", "results/results-16.txt")
