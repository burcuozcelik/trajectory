from __future__ import division
import os
import sys

cg_home=os.environ["SRC_HOME"]
ml_training=cg_home + "/results/multibit/ml-80/training/"
ml_testing=cg_home + "/results/multibit/ml-80/testing/"
sbatchdir="./sbatchs_multi/training-80/"
#models=['Bagging',  'DecisionTree', 'RandomForest','ExtraTrees','AdaBoost', 'GradientBoosting', 'GaussianNB', 'SVM', 'MLPC']
models=['Bagging',  'DecisionTree', 'RandomForest','ExtraTrees', 'GradientBoosting', 'GaussianNB', 'MLPC']  

def main():
    fid=0
    for path, subdirs, files in os.walk(ml_training):
        for dfile in files:
            fname=dfile.split(".")[0]
            for model in models:
                sbatchfname = fname + "_" + model 
                fsbatch = open(sbatchdir + sbatchfname +".sh", "w")
                fsbatch.write("#!/bin/bash\n")
                fsbatch.write("#SBATCH --partition=pal\n")
                fsbatch.write("#SBATCH --account pal\n")
                fsbatch.write("#SBATCH -N 1\n")
                fsbatch.write("#SBATCH -e "+ sbatchfname + ".err\n")
                fsbatch.write("#SBATCH -o "+sbatchfname + ".out\n")

                cline = "python classifiers_wstages.py -s 4 -t " + ml_training + fname + ".train -x " +ml_testing + fname + ".test -i " + model + "\n"
                ofile = ml_training + fname + "_" + model + ".model"
                fsbatch.write("if [ ! -f " + ofile + " ]\n")
                fsbatch.write("then\n")
                fsbatch.write("\t" + cline)
                fsbatch.write("elif [ \"$( cat "+ ofile + " | wc -l )\" == \"0\" ] \n")
                fsbatch.write("then\n")
                fsbatch.write("\t" + cline)
                fsbatch.write("fi\n\n")

main()

