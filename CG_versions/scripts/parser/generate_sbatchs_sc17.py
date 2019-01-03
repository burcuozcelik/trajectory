from __future__ import division
import os
import sys
from sys import argv


training_classes=["1", "2"]
training_types=["normal", "dual"]
training_version=["chunk10", "last10"]
numsamples= ["1000", "2000", "4000", "6000", "8000", "10000", "12000", "14000", "16000"]
solvers=["cg", "cgs", "iccg", "bicg", "bicgsta", "qmr"]
feature_types=["woDetectors", "wDetectors"]
testing_options=["all" , "sign", "exponent", "mantissa", "sign"]

training_basedir="../data/datasets/memory/training/"
testing_basedir="../data/datasets/memory/testing/"
result_basedir="../data/model/"
sbatch="./sbatchs/"

for solver in solvers:
	for tversion in training_version:
		for ftype in feature_types:
			for tclass in training_classes:
				for ttype in training_types:
					for tsize in numsamples:
						training=training_basedir+solver+"_"+tversion+"_"+ftype+"_train_"+tclass+"_"+ttype+"_"+tsize+".dat"
						print training
						for toption in testing_options:
							testing=testing_basedir+solver+"_"+tversion+"_"+ftype+"_test_"+tclass+"_"+ttype+"_"+toption+"_"+tsize+".dat"
							print testing
							fname=solver+"_"+tversion+"_"+ftype+"_"+tclass+"_"+ttype+"_"+toption+"_"+tsize
							print fname
							with open(sbatch+fname+".sh",'w') as fp:
								fp.write("#!/bin/bash\n")
								fp.write("#SBATCH --partition=pal\n")
								fp.write("#SBATCH --account pal\n")
								fp.write("#SBATCH -N 1\n")
								fp.write("#SBATCH -e "+fname+".err\n")
								fp.write("#SBATCH -o /dev/null\n")
								fp.write("python ../classifiers.py "+training+" "+testing+" > "+result_basedir+fname+".out\n")

			
		




	
        

