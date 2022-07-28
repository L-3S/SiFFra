# -*- coding: utf-8 -*-
"""
Created on Wed Jul 27 20:52:08 2022

@author: NV261592
"""

import numpy as np
import matplotlib.pyplot as plt


X0 = 0.
K = 1.
tau = 10.
dt=1.
l_t = 1000
X = np.zeros(l_t)
tt = np.array(range(l_t))
X[0] = X0
for ii in range(1, l_t):
    X[ii] = X[ii-1]*(1. - dt/tau) + K *dt / tau
    
    
plt.plot(tt, X)
plt.show()
