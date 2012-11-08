#!/usr/bin/env python3

from itertools import groupby

wordmap = {}
def index(word):
    if word not in wordmap:
        wordmap[word] = len(wordmap)
    return wordmap[word]

with open('data.txt') as f:
    count = int(f.readline())
    for i in range(count):
        words = f.readline().split()
        groups = groupby(words, key = index)
        print(' '.join('%s:%s' % (k, len(list(g))) for k, g in groups))

import pickle
pickle.dump(wordmap, open('wordmap', 'wb'))
