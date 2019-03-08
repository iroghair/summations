# Summations 

## Intro
It has been found that a parallel implementation of the DPM did not provide
the same (exact) results as its serial counterpart. We suspected that the 
addition of finite precision floating point numbers would be the root cause.
The (serial) DPM model loops over all the Lagrangian particles in sequence 
to account for their contribution to various field variables, e.g. porosity, 
pressure, momentum exchange etc. The parallel code adds these contributions
in a different order, and this accounts for a numerical error that perturbs
the simulation so that the results are not exactly identical compared to the
serial code. As a matter of fact, multiple runs of the same simulation may
yield slightly different results due to different scheduling.

## Components
* Python script `generateBinaryPositions.py`: Generates a binary file that 
contains the number of particles (1 integer) and consequently their (x,y,z) 
positions.
* Binary file `example.bin`: file containing the integer 10 000 000, followed
by 30 000 000 binary doubles (i.e. 10M (x y z) positions).
* A main file `main.cpp` that loads the binary file into an N x 3 array, and
proceeds to sum all the positions to a single summation:
```math
    s = \sum_{n=0}^{N_\mathrm{part}} x_n + y_n + z_n
```
It accounts for a summation using OpenMP parallelization, a serial summation
(counting from 0 up to npart), another serial (counting down from npart to 0) 
and finally a sum that accounts for minimizing the numerical error using the
 [Kahan summation algorithm](https://en.wikipedia.org/wiki/Kahan_summation_algorithm)
The program also performs a quad-precision summation which is printed first, 
and the differences between the 4 double-precision summation techniques are 
provided behind the results.

## Sample output
The program loads an input file given as argument to the program:
```
$ ./summation example.bin
Trying to open file example.bin ...
Trying to read 10000000 positions from file...
The truth, and nothing but the truth (i.e. quad precision):
                         1.499928871027937014807207560807e+07
Four different summations with double:
 OpenMP:                1.4999288710279474e+07 (delta: -1.0430812835693359e-07)
 Serial 0->nparts:      1.4999288710278464e+07 (delta: +9.0524554252624512e-07)
 Serial nparts-->0:     1.4999288710281001e+07 (delta: -1.6316771507263184e-06)
 Kahan:                 1.4999288710279370e+07 (delta: +0.0000000000000000e+00)
Exiting normally...
```

## To do
