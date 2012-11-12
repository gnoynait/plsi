#!/bin/sh

OUT=likelihoods.txt
rm $OUT

for Z in 10 20 50 100 200 500 1000 2000 5000
do
    cat train_${Z}_200.log | awk '/^iteration/{print $4}' | tr '\n' ' ' >> $OUT
    echo >> $OUT
done
