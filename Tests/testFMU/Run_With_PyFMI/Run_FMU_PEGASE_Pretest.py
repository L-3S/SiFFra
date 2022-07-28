# -*- coding: utf-8 -*-
"""
Created on Thu Feb  1 09:38:35 2018

"""
# Import the load function (load_fmu)
from pyfmi import load_fmu
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

#Load the FMU
model = load_fmu('./../fmus/TestFMU_TestSaveRestoreState_Crash2_Radau.fmu')
#model = load_fmu('Emulate_PythonModule_TNR_V2.fmu')
# model = load_fmu('Pegase_Cosim_FMU_Dymola.fmu')

opts = model.simulate_options()

res = model.simulate(final_time=2.45, options={'ncp': 500})
#res = model.simulate(final_time=30, options={'ncp': 300})
print(res['time'])
print(res['Corr'])
print(res.keys())

plt.step(res['time'], res['State2Real'],  'r', where='post')
plt.plot(res['time'], res['State2Real'], 'rx')
plt.step(res['time'], res['State3Real'], 'b',  where='post')
plt.plot(res['time'], res['State3Real'], 'bx')
plt.show()
plt.clf() 
plt.step(res['time'], res['realExpression1.y'],  'g', where='post')
plt.plot(res['time'], res['realExpression1.y'],  'gx')
plt.show()
plt.clf()
plt.step(res['time'], res['OutPutTimeIntegrator'], 'g', where='post')
plt.plot(res['time'], res['OutPutTimeIntegrator'], 'gx')
plt.show()
plt.clf()