#!/usr/bin/env python

import numpy as np
from array import array

npart = np.intc(100000)
fname = 'example.bin'

print ("Generating {} particle positions...".format(npart))

a = np.random.rand(npart, 3)

np.set_printoptions(precision=16)
print(a)

print ("Writing to file ....")
output_file = open(fname, 'wb')
output_file.write(npart.tobytes())
output_file.write(a.tobytes())
output_file.close()
print ("Done!")





