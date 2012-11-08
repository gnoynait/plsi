#!/usr/bin/env python3

import pickle
import sys

wzfile = sys.argv[1] if len(sys.argv) > 1 else 'wz'
to_list = int(sys.argv[2]) if len(sys.argv) > 2 else 20

wordmap = pickle.load(open('wordmap', 'rb'))
word = [None] * len(wordmap)
for k, v in wordmap.items():
	word[v] = k

with open(wzfile) as f:
	for i, l in enumerate(f):
		print("topic", i)
		p = list(map(float, l.split()))
		index = list(range(len(p)))
		index.sort(key = lambda i: p[i], reverse = True)
		for i in index[:to_list]:
			print('\t', word[i], p[i])
		print()