A simple PLSI Implementation
============================

Feature Extraction
------------------

It's in preprocess.py. It will count term-frenquency for each document. The output is like:

    1:2 2:3
    2:1 3:2 4:5

e.g. each document is represented with a list of 'word-id:term-frequency'.

Training
--------

Using the E-M method.

E-step: update P(z|d,w)
M-step: update P(w|z), P(d|z), P(z)

In fact, we won't store P(z|d,w) as it'll be too large, besides it's unnecessary. We can loop the documents and words and do the M-step in the same time. And if the RAM is sufficient, we can update P(w|z), P(d|z) and P(z) together.

Evaluation
----------

Randomly split the docs into training(90%) and testing(10%). Calculate the perplexity for each k from 10 to 200.

Results
-------

Top words for each topic are collected in 'topics.txt'. A chart for 'topics and perplexity' are drew in the 'k-p.png'.

How to run
----------

Use ``preprocess.py`` to extract features. Use ``split.py`` to split docs. Then ``make`` and run ``./plsi``. You can specific topics and max iterations with environment variables:

    Z=10 IMAX=10000 ./plsi

After that, use ``evaluate.py`` to calculate perplexity for the test doc.

Tested under ArchLinux, gcc 4.7.2, python3.3.
