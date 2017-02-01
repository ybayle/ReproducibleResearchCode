# -*- coding: utf-8 -*-
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   06/01/2017
# Updated   06/01/2017
# Version   1.0.0
#
"""
Description of stats.py
======================

1 Shapiro (test normality)
2 Bartlett (test homostedasticity)
3 if normal and same variance then ANOVA and Tukey post hoc test
  else kruskall wallis

TABLE 1
/media/sf_github/array1/repro.py : ghosal()
/media/sf_github/kea/kea.py figure2()
/media/sf_github/vqmm2/vqmm2.py figure1()

"""

import sys
import numpy as np
from scipy import stats
from statsmodels.stats.multicomp import MultiComparison
from statsmodels.stats.multicomp import pairwise_tukeyhsd

def assert_normality(data):
    """Description of assert_normality
    The Shapiro-Wilk test tests the null hypothesis that the data was drawn 
    from a normal distribution
    if test_stat and p_value close to 1 then data is normal
    """
    print("Values " + str(data))
    statistic, pvalue = stats.shapiro(data)
    print("Shapiro Statistic " + str(statistic) + " and p-value " + str(pvalue))
    if pvalue > 0.05:
        print("Normal")
        return True
    else:
        print("Not normal")
        return False

def bartlett(data):
    """Description of bartlett
https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.bartlett.html
    """
    if len(data) == 3:
        statistic, pvalue = stats.bartlett(data[0], data[1], data[2])
    else:
        print(data)
        print("TODO barlett not 3 values")
    print("Bartlett Statistic " + str(statistic) + " and p-value " + str(pvalue))
    if pvalue > 0.05:
        return True
    else:
        return False

def assert_homoscedasticity(data):
    """Description of assert_homoscedasticity
    Bartlett’s test tests the null hypothesis that all input samples are from
    populations with equal variances. For samples from significantly non-normal
    populations, Levene’s test levene is used.
    """
    normality = True
    for item in data:
        normality = assert_normality(item)
        if not normality:
            break
    if normality:
        return bartlett(data)
    else:
        print("TODO levene")

def anova(data):
    """
    return True is at least one mean is different from the other
https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.f_oneway.html
    """
    if len(data) == 3:
        statistic, pvalue = stats.f_oneway(data[0], data[1], data[2])
    else:
        print("TODO anova not 3 values")
    print("ANOVA Statistic " + str(statistic) + " and p-value " + str(pvalue))
    if pvalue < 0.05:
        return True
    else:
        return False

def tukey(data, names):
    names = np.array(names)
    tmp = []
    for item in data:
        for val in item:
            tmp.append(val)
    data = np.array(tmp)
    mc = MultiComparison(data, names)
    result = mc.tukeyhsd()
    print(result)
    print(mc.groupsunique)

def kruskal(data):
    """
https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.kruskal.html
    """
    if len(data) == 3:
        statistic, pvalue = stats.kruskal(data[0], data[1], data[2])
    else:
        print("TODO kruskal not 3 values")
    print("Kruskal Statistic " + str(statistic) + " and p-value " + str(pvalue))
    if pvalue > 0.05:
        # same
        return False
    else:
        # different
        return True

def main():
    data = []
    names = []
    with open("table2_f1.csv", "r") as filep:
        # next(filep)
        for line in filep:
            row = line[:-1].split(",")
            tmp = []
            for index in range(1, len(row)):
                names.append(row[0])
                tmp.append(float(row[index]))
            data.append(tmp)
    # with open("table2_f1.csv", "r") as filep:
    #     next(filep)
    #     tmp = []
    #     for line in filep:
    #         row = line[:-1].split(",")
    #         # print("row[1] " + str(row[0]) + " names[:-2] " + str(names[-2]) + " " + str(row[0] != names[:-2]))
    #         if len(names) > 3 and row[0] != names[-1]:
    #             data.append(tmp)
    #             tmp = []
    #         tmp.append(float(row[1]))
    #         names.append(row[0])
    # print(names)
    # print(data)
    # data = [[10, 1000, 300000, 400], [1, 1.1, 1.2, 1.05]]
    # data = [[9, 9.1, 9.2, 9.3], [1, 1.1, 1.2, 1.05]]
    # data = [[1, 1.1, 1.2, 1.05, 1.06], [1, 1.1, 1.2, 1.05]]
    if assert_homoscedasticity(data):
        if anova(data):
            tukey(data, names)
        else:
            print("All means are the same")
    else:
        if kruskal(data):
            print("cf R")
            # Dunn
            # Conover-Iman
            # Dwass-Steel-Citchlow-Fligner
        else:
            print("All means are the same")

if __name__ == "__main__":
    main()
