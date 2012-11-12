#!/usr/bin/env python3

from matplotlib.pyplot import *

l = open('likelihoods.txt').readlines()
ll = list(map(lambda _: list(map(float, _.split())), l))
legends = [10, 20, 50, 100, 200, 500, 1000, 2000, 5000]
for i, x in enumerate(ll):
    plot(x, label = "Z = %s" % legends[i])
xlabel("iteration")
ylabel("log-likelihood")
legend()
show()
