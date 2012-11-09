#!/usr/bin/env python3
from math import log, exp

def read():
    p_z = list(map(float, open('z').readlines()))
    p_wz = []
    for l in open('wz'):
        p_wz.append(list(map(float, l.split())))

    docs = []
    for l in open('docs_test'):
        doc = dict(map(int, k.split(':')) for k in l.split())
        docs.append(doc)

    return docs, p_z, p_wz

def perplexity(docs, p_z, p_wz):
    denominator = sum(sum(d.values()) for d in docs)
    nominator = 0.0
    for d in docs:
        p_w = 1
        for w, tf in d.items():
            r = 0
            for zi in range(len(p_z)):
                r += p_wz[zi][w] * p_z[zi]
            p_w *= r
        nominator += log(p_w, 2)
    perplexity = exp(-nominator / denominator)
    return perplexity

print('perplexity:', perplexity(*read()))
