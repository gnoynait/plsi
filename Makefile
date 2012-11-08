plsi: plsi.cpp
	g++ --std=c++11 -o $@ -O3 -Wall -DNDEBUG $<

plsi-debug: plsi.cpp
	g++ --std=c++11 -g -o $@ -O0 -Wall $<
