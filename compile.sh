#!/bin/bash
echo "1/3..."
g++ resource_cycle_finder.cpp -fopenmp -lyaml-cpp -std=c++20 -o finder_p
echo "2/3..."
g++ resource_cycle_finder.cpp  -lyaml-cpp -std=c++20 -O2 -o finder_O2
g++ resource_cycle_finder.cpp -fopenmp -lyaml-cpp -std=c++20 -O2 -o finder_O2p
echo "3/3..."
g++ resource_cycle_finder.cpp -fopenmp -lyaml-cpp -std=c++20 -O2 -march=native -o finder_O2mp
