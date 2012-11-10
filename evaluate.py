#!/usr/bin/env python3
from math import log, exp

def read():
    p_z = list(map(float, open('z').readlines()))
    p_wz = []
    for l in open('wz'):
        p_wz.append(list(map(float, l.split())))
    w = len(p_wz[0])
    p_w = [0] * w
    for zi, wz in enumerate(p_wz):
        for wi, p in enumerate(wz):
            p_w[wi] += p * p_z[zi]
    docs = []
    for l in open('docs_test'):
        features = [map(int, k.split(':')) for k in l.split()]
        doc = dict((k, v) for k, v in features if k < w)
        docs.append(doc)

    return docs, p_w

def perplexity(docs, p_w):
    nominator = 0.0
    denominator = 0
    for d in docs:
        for wi, tf in d.items():
            if p_w[wi] > 0:
                nominator += tf * log(p_w[wi], 2)
                denominator += tf
    print(nominator, denominator)
    perplexity = exp(-nominator / denominator)
    return perplexity

print('perplexity:', perplexity(*read()))
