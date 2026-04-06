#!usr/bin/python

import numpy as np

a = np.arange(24).reshape(6, 4)
print(a)
# [[ 0  1  2  3]    0
#  [ 4  5  6  7]    1
#  [ 8  9 10 11]    2
#  [12 13 14 15]    3
#  [16 17 18 19]    4
#  [20 21 22 23]]   5

aa = np.delete(a, np.s_[2:6], 0)
print( len(aa) )

print( aa[0] )
