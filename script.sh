#!/bin/bash
for run in {1..100};
do ./build/single_threaded -s;
done

for run in {1..100};
do ./build/multithreaded 2 -s;
done

for run in {1..100};
do ./build/multithreaded 3 -s;
done

for run in {1..100};
do ./build/multithreaded 4 -s;
done

for run in {1..100};
do ./build/multithreaded 5 -s;
done

for run in {1..100};
do ./build/multithreaded 6 -s;
done

for run in {1..100};
do ./build/multithreaded 7 -s;
done

for run in {1..100};
do ./build/multithreaded 8 -s;
done

for run in {1..100};
do ./build/multithreaded 16 -s;
done

for run in {1..100};
do ./build/multithreaded 32 -s;
done

for run in {1..100};
do ./build/multithreaded 64 -s;
done

for run in {1..100};
do ./build/multithreaded 128 -s;
done