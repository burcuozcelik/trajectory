from __future__ import division
import os                                                                                                                                                                                    
import sys      
import numpy as np
import random   
import os.path
from scipy.stats import beta
from scipy.stats import norm
from scipy.stats import uniform


solvers = ['bicg', 'bicgsta', 'cg' , 'cgs', 'iccg']
datasets = ['af_shell3', 'af_shell4', 'af_shell7', 'af_shell8', 'bcsstk13', 'bcsstk14', 'bcsstk15', 'bcsstk16', 'bcsstk24', 'bcsstk27', 'bcsstk28', 'bcsstk38', 'ex3', 'ex9', 'ex13', 'ex15', 'Kuu', 'msc04515', 'nasa2146', 'Pres_Poisson', 'sts4098', 's1rmq4m1', 's1rmt3m1', 's2rmq4m1', 's2rmt3m1', 's3rmq4m1', 's3rmt3m1', 's3rmt3m3']

num_rows=['504855', '504855', '504855', '504855', '2003', '1806', '3948', '4884', '3562', '1224', '4410', '8032', '1821', '3363', '2568','6867','7102', '4515',  '2146', '14822', '4098', '5489', '5489', '5489','5489', '5489', '5489', '5357']

solver_iterations=[['1117', '524', '1117', '1214', '572'],
                   ['1117', '524', '1117', '1214', '572'],
                   ['1118', '466', '1118', '1327', '571'],
                   ['1118', '466', '1118', '1327', '571'],
                   ['928', '462', '928', '1185', '330'],
                   ['195', '108', '195', '108', '101'],
                   ['453', '198', '453', '207', '166'],
                   ['148', '95', '148', '94', '49'],
                   ['451', '711', '451', '374', '727'],
                   ['185', '142', '185', '155', '59'],
                   ['4344', '16226', '4344', '7381', '1309'],
                   ['426', '156', '426', '1218', '119'],
                   ['181', '155', '181', '3625', '123'],
                   ['153', '181', '153', '-1', '44'],
                   ['146', '101', '146', '104', '34'],
                   ['96', '72', '96', '82', '33'],
                   ['378', '260', '378', '-1', '116'],
                   ['2169', '3580', '2169', '-1', '1176'],
                   ['171', '116', '171', '99', '56'],
                   ['662', '709', '662', '669', '218'],
                   ['244', '158', '244', '219', '96'],
                   ['612', '489', '612', '639', '172'],
                   ['695', '576', '695', '683', '205'],
                   ['1237', '422', '1237', '-1', '546'],
                   ['1787', '1733', '1787', '2085', '657'],
                   ['2969', '2096', '2969', '2530', '1274'],
                   ['4497', '1937', '4497', '4514', '2111'],
                   ['8538', '5037', '8538', '12097', '2487']]


distrubutions=['normal', 'uniform', 'beta51']
numbitflips=[['1', '2', '4', '6'],
          ['1', '2', '4', '6'],
          ['1', '2', '4', '6']]

#distrubutions=['normal']
#numbitflips=[['-1']]


np.random.seed(1)
num_iterations=2000
for isolver in range(0,len(solvers)):
   for idataset in range (0, len(datasets)):
      for idis in range (0, len(distrubutions)):
         dis_bitflips=numbitflips[idis]
         for ibitflip in range (0, len(dis_bitflips)):
            if numbitflips[idis][ibitflip] != "-1":
               fname= solvers[isolver] + "_" + datasets[idataset] + "_" + distrubutions[idis] + "_" + numbitflips[idis][ibitflip] + "bit" + ".rnd"
               print fname
               context = ""
               for niter in range(0,num_iterations):
                  context = context + str(int((np.random.uniform(1, sys.maxint) % int(solver_iterations[idataset][isolver]))) + 1) + ";"
                  context = context + str(int(np.random.uniform(1, sys.maxint) % int(num_rows[idataset]))) + ";"

                  bflips=[]
                  #for nbf in range(0,int(numbitflips[idis][ibitflip])):
                  while len(bflips) != int(numbitflips[idis][ibitflip]):
                     if distrubutions[idis] == 'normal':
                        rvalue=int(random.gauss(32,8))
                     elif distrubutions[idis] == 'uniform':
                        rvalue=int(64*random.uniform(0,1))
                     elif distrubutions[idis] == 'beta51':
                        rvalue=int(64*random.betavariate(5,1))
                     
                     if not rvalue in  bflips:
                        bflips.append(rvalue)
                  
                  for vbf in range(0,len(bflips)):
                     context = context + str(bflips[vbf]) + ";"

                  context = context + "\n"
               
               if os.path.exists(fname):
                  print 'File exist ', fname  
               else:
                  with open(fname,'w') as dfile:
                     dfile.write(context)



###################################Joseph's version##########################
#import os
#import sys
#import random

#list_time = [0,1,2,3,4,5,6]
#list_position = [0.0,0.2,0.4,0.6,0.8,1.0]
#reps = 10

#for x in range(10):
#   for y in list_position:
#      for z in list_time:
#         name = "nums/rand_" + str(z) + "_" + str(y) + "_" + str(x) + ".rd"
#         f = open(name, "w+")
#         print ("Name: ", name)
#         rn = random.getrandbits(64)
#         print ("Number ", rn)
#         f.write(str(rn))
#         
#f.close()         

