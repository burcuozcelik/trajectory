from __future__ import division
import os
import sys
import os
import glob
from sys import argv


solvers = ['cg']
datasets = ['ex9', 'ex13', 'ex15']
distrubutions = ['uniform']
numbitflips = [['1','2','4']]
dirname=['results']
execname=['mlpredict']


if len(argv) < 4:
        print("Usage: experiment_start_point; experiment_end_point; number_of_experiments_in_sbatch")
        sys.exit(1)

experiment_start_point=int(argv[1])
experiment_end_point=int(argv[2])
nrum_in_each_sbatch=int(argv[3])

cg_home=os.environ["SRC_HOME"]
dirinject= cg_home + "/src/global/nums/" 
sbatchdir="./sbatchs_multi/mlpredict"
dirstatements = cg_home + "/inc/"
pklname = 'cg_randomforest_100_masked.pkl'
#cleanup previous files
rmfiles = glob.glob(sbatchdir + "/*.sh")
for f in rmfiles:
	os.remove(f)

##baseline file
#for isolver in range(0,len(solvers)):
#	fsbatch = open(sbatchdir +  "/" + solvers[isolver] + "_baselines.sh" , "w")
#	fsbatch.write("#!/bin/bash\n")  
#	fsbatch.write("#SBATCH --partition=pal\n") 
#	fsbatch.write("#SBATCH --account pal\n")  
#	fsbatch.write("#SBATCH -N 1\n") 
#	fsbatch.write("#SBATCH -e "+ solvers[isolver] + "_baselines" +".err\n")  
#	fsbatch.write("#SBATCH -o /dev/null\n\n")
#											
#	for idataset in range (0, len(datasets)):
#		cline="./" + solvers[isolver] + "_" + "baseconv" + " ./datasets-all/" + datasets[idataset] + "/" + datasets[idataset] + ".rb "
#		cline = cline + "0 0 0 -1 0 0 0 "
#		ofile=cg_home + "/results/convergence/baseline/"+ solvers[isolver] + "/" + datasets[idataset] + "/" + solvers[isolver] +"_"+ datasets[idataset] + "_baseline.dat"
#		fsbatch.write("if [ ! -f " + ofile + " ]\n")
#		fsbatch.write("then\n")
#		fsbatch.write("\t" + cline + " > " + ofile + " &\n")
#		fsbatch.write("elif [ \"$( grep -re \"x_vector\" "+ ofile + " | wc -l )\" == \"0\" ] \n")
#		fsbatch.write("then\n")
#		fsbatch.write("\t" + cline + " > " + ofile + " &\n")
#		fsbatch.write("fi\n\n")
#	fsbatch.write("wait\n")
#	fsbatch.close()	
#end baseline

for dindex in range(0, len(dirname)):
	for isolver in range(0,len(solvers)):
		for idataset in range (0, len(datasets)):
			for idis in range (0, len(distrubutions)):
				dis_bitflips=numbitflips[idis]
				for ibitflip in range (0, len(dis_bitflips)):
					if numbitflips[idis][ibitflip] != "-1":
						fname= solvers[isolver] + "_" + datasets[idataset] + "_" + distrubutions[idis] + "_" + numbitflips[idis][ibitflip] + "bit"
						with open(dirstatements + solvers[isolver] + "/" + solvers[isolver] + "_converge.dat",'r') as st:
							slines = st.readlines();
							for sl in slines:
								stnum = sl.split(';')[0]
								vecs = sl.split(';')[1:-1:]
								for ivec in vecs:
									#for cg and iccg, p,r,x vecid's will change , 
									# p:0->0 , r:2->1 , x:3->2 
									if ivec=='2':
										ivec = '1'
									elif ivec == '3':
										ivec = '2'
									fstname = fname + "_" + stnum + "_" + ivec 
									with open(dirinject + fname + ".rnd",'r') as fp:
										lines = fp.readlines()
										for iindex in range(experiment_start_point,experiment_end_point):
											if iindex % nrum_in_each_sbatch == 0:
												print fstname + "_" + str(int(iindex/nrum_in_each_sbatch))
												fsbatch = open(sbatchdir + "/" + fstname + "_" + str(int(iindex/nrum_in_each_sbatch)) + ".sh" , "w")
												fsbatch.write("#!/bin/bash\n")  
												fsbatch.write("#SBATCH --partition=pal\n") 
												fsbatch.write("#SBATCH --account pal\n")  
												fsbatch.write("#SBATCH -N 1\n") 
												fsbatch.write("#SBATCH -e "+fstname + "_" + str(int(iindex/nrum_in_each_sbatch))+".err\n")  
												fsbatch.write("#SBATCH -o /dev/null\n")
											
											params=lines[iindex].split(";")

											cline="./" + solvers[isolver] + "_" + execname[dindex] + " ./datasets-all/" + datasets[idataset] + "/" + datasets[idataset] + ".rb "
											if dirname[dindex] == "baseline":
												cline = cline + "0 0 0 0 0"
											else:
												numbf= len(params) - 3
												cline = cline + "1 " + stnum + " " + ivec + " " + params[0] + " " + params[1] + " " + str(numbf)
												for bf in range(0,numbf):
													cline = cline + " " + params[2+bf]
												cline = cline + " " + pklname

											ofile=cg_home + "/results/mlpredict/" + dirname[dindex] + "/"+ solvers[isolver] + "/" + datasets[idataset] + "/" + fstname + "_" + str(int(iindex/nrum_in_each_sbatch)) + "_" + str(iindex % nrum_in_each_sbatch) + ".dat"
											fsbatch.write("if [ ! -f " + ofile + " ]\n")
											fsbatch.write("then\n")
											fsbatch.write("\t" + cline + " > " + ofile + " &\n")
											if dirname[dindex] == "data":
												fsbatch.write("elif [ \"$( grep -re \"x_vector\" "+ ofile + " | wc -l )\" == \"0\" ] \n")
												fsbatch.write("then\n")
												fsbatch.write("\t" + cline + " > " + ofile + " &\n")
											fsbatch.write("fi\n\n")

											if iindex % nrum_in_each_sbatch == nrum_in_each_sbatch - 1:
												fsbatch.write("wait\n")
			
								#if iindex == num_iterations:
								#	break
								
		




	
        

