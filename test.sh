#!/bin/sh

IMAX=200

for Z in 10 20 50 100 200 500 1000 2000 5000; do
    export IMAX
    export Z
    ./plsi | tee train_${Z}_200.log
done
