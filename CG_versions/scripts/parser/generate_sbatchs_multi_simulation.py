from __future__ import division
import os
import sys
import os
import glob
from sys import argv
import uuid

dirname="simulation"
cg_home=os.environ["SRC_HOME"]
#datadir=cg_home + "/results/multibit/baseline/"
#simulationdir=cg_home + "/results/multibit/simulation-baseline/"
#sbatchdir="./sbatchs_multi/simulation-baseline/"

datadir=cg_home + "/results/multibit/data/"
simulationdir=cg_home + "/results/multibit/simulation/"
sbatchdir="./sbatchs_multi/simulation/" 

num_included=100
max_limit=num_included*10
detectors = ['aid', 'mad', 'newsum', 'orth']

from glob import glob
from os import rename
 
def parse_data(fname):
	newname=""
	ready=0
	ofname=""
	solver=""
	#where=""
	#when=""
	#bits=[]
	with open(fname, 'rb') as f:
		#for line in f:

                lines = f.readlines()
		
		if lines:
			when =  lines[-4][34:-1:]
			where = lines[-3][25:-1:]
			bits = lines[-2][11:-2:].split(" ")
			
			farray = fname.split("_")
			for i in range(0, len(farray)-3):
				newname = newname + farray[i] + "_"

			newname = newname + when + "_" + where + "_" + farray[-3]
          
			for bit in bits:
				newname = newname + "_" + bit
			
			ofname=newname.split("/")[-1] 

			farray = ofname.split("_")                                                                              
			solver = farray[0]  
		
		ready=0
		#lines = f.readlines() 
		for line in lines:
			if "NEWSUM_SCALER" in line:
				ready=1
				break

		#	if not "baseline" in fname:
		#		if "Place in iterations space (when)" in line:
		#			when=line[34:-1:]
		#		elif "Place in vector (where)" in line:
		#			where = line[25:-1:]  
		#		elif "Bit flips:" in line:
		#			bits = line[11:-2:].split(" ")

		#fname = "bicgsta_bcsstk24_baseline.dat"
		#if ready == 1:
		#	if "baseline" in fname:
		#		farray = fname.split("/")
		#		farray = farray[-1].split(".")
		#		ofname = farray[0]
			
		#	else:
		#		farray = fname.split("_")
		#		for i in range(0, len(farray)-3): 
		#			newname = newname + farray[i] + "_" 
			
				#print fname
		#		newname = newname + when + "_" + where + "_" + farray[-3] 
		#		for bit in bits:
		#			newname = newname + "_" + bit

		#		ofname=newname.split("/")[-1]

		#	farray = ofname.split("_")                                                                       
		#	solver = farray[0]  

	#print ofname
	#print solver
	return {'ready':ready,'ofname':ofname, 'solver':solver}


def find_mad_wsize(fname):
	#window size 3 means that we could not find window size that results zero false positives
	datasets= ['af_shell3', 'af_shell4', 'af_shell7', 'af_shell8', 'aft01', 'bcsstk13', 'bcsstk14', 'bcsstk15', 'bcsstk16', 'bcsstk24', 'bcsstk27', 'bcsstk28', 'bcsstk38', 'crystm01', 'ex13', 'ex15', 'ex3', 'ex9', 'Kuu', 'msc04515', 'Muu', 'nasa2146', 'Pres_Poisson', 's1rmq4m1', 's1rmt3m1', 's2rmq4m1', 's2rmt3m1', 's3rmq4m1', 's3rmt3m1', 's3rmt3m3', 'sts4098']
	mad_ws_cg=['60', '60', '60', '60', '1', '3', '180', '3', '20', '3', '20', '3', '3', '2', '20', '60', '160', '40', '80', '3', '2', '20', '540', '600', '380', '920', '3', '2960', '3', '3', '240']
        mad_ws_cgs=['3', '3', '3', '3', '1', '3', '3', '3', '3', '3', '3', '7380', '3', '2', '3', '3', '3', '50000', '3', '50000', '2', '3', '3', '3', '3', '3', '3', '3', '3', '3', '3']
        mad_ws_iccg=['60', '60', '60', '60', '1', '3', '80', '140', '20', '720', '20', '1300', '3', '2', '20', '20', '3', '40', '60', '1120', '2', '20', '120', '120', '120', '500', '580', '3', '3', '3', '3']
        mad_ws_bicg=['60', '60', '60', '60', '1', '3', '180', '3', '20', '3', '20', '3', '3', '2', '20', '60', '160', '40', '80', '3', '2', '20', '540', '600', '380', '920', '3', '2960', '3', '3', '240']
        mad_ws_bicgsta=['3', '3', '3', '3', '1', '440', '3', '3', '80', '3', '3', '3', '3', '2', '100', '3', '3', '180', '260', '3580', '2', '3', '3', '3', '3', '420', '3', '3', '3', '3', '3']

	 
	farray = fname.split("_")
	solver = farray[0]
	dataset= farray[1]
	if dataset == "af" or dataset == "Pres":
		dataset= dataset + "_" + farray[2]

	idataset = datasets.index(dataset)
	if solver == "cg":
		return mad_ws_cg[idataset]
	elif solver == "cgs":
		return mad_ws_cgs[idataset]
	elif solver == "iccg":
                return mad_ws_iccg[idataset]
	elif solver == "bicg":
                return mad_ws_bicg[idataset]
	elif solver == "bicgsta":
                return mad_ws_bicgsta[idataset]


def dump_output():
	fmissing = open("missing_data_files.out", "w")
	fmissing.write("Total number of files: " + str(num_all_files) + "\n")
        fmissing.write("Number of files completed: " + str(num_completed_files) + "\n")
        for mfile in missing_files:                                  
		fmissing.write(mfile + "\n")
		os.remove(mfile)

def write_to_file_for_ml(solver, inputs, outputs):
	global fid
	ds=['1', '4', '8', '16', '28']
	cg_models=['SVM', 'ExtraTrees', 'ExtraTrees', 'ExtraTrees', 'ExtraTrees']
	cgs_models=['Bagging', 'Bagging', 'ExtraTrees', 'ExtraTrees', 'ExtraTrees']
	iccg_models=['MLPC', 'MLPC', 'RandomForest', 'RandomForest', 'ExtraTrees']
	bicg_models=['Bagging', 'ExtraTrees', 'ExtraTrees', 'ExtraTrees', 'ExtraTrees']
	bicgsta_models=['Bagging', 'ExtraTrees', 'Bagging', 'ExtraTrees', 'ExtraTrees']

	#ds=['16', '28']
	#cg_models=['Bagging', 'ExtraTrees']   
	#cgs_models=['RandomForest', 'ExtraTrees'] 
	#iccg_models=['ExtraTrees', 'ExtraTrees'] 
	#bicg_models=['Bagging', 'ExtraTrees']   
	#bicgsta_models=['ExtraTrees', 'ExtraTrees']  
	
	if solver == 'cg':
		models = cg_models
	elif solver == 'cgs':
		models = cgs_models
        elif solver == 'iccg':
		models = iccg_models
        elif solver == 'bicg':
		models = bicg_models
	elif solver == 'bicgsta':
		models = bicg_models

	for mindex in range(0,len(ds)):
		for rifile in range(0, len(inputs),num_included):
			sbatchfname= "ml_DS" + ds[mindex]  + "_sbatch_" + str(fid)
			fid = fid + 1
			fsbatch = open(sbatchdir + "ml/" + sbatchfname + ".sh", "w")
			#fsbatch = open(sbatchdir  + sbatchfname + ".sh", "w")  
			fsbatch.write("#!/bin/bash\n")
			fsbatch.write("#SBATCH --partition=pal\n")
			fsbatch.write("#SBATCH --account pal\n")
			fsbatch.write("#SBATCH -N 1\n")
			fsbatch.write("#SBATCH -e "+ sbatchfname + ".err\n")
			fsbatch.write("#SBATCH -o /dev/null\n")
			
			start_point=rifile;
			end_point=rifile + num_included 
			if end_point > len(inputs):
				end_point = len(inputs)
			
			mname= solver +"_normal_numDS" + ds[mindex]+".train_"+models[mindex]+".model"
			cline = "python static_detect.py ml " + mname + " " + str(end_point - start_point)
			for index in range(start_point, end_point):
				fin=inputs[index].split('/')[-1]
				cline = cline + " " + fin + " " + outputs[index] + ".out "
			fsbatch.write(cline + "\n")	

def write_to_file_for_detectors(ready_for_execution_input, ready_for_execution_output):
	global fid
	aid_thr=['0.00078125']
        newsum_thr=['0.0000000001']
        orth_thr=['0.0000000001']
        mad_thr=['0.1']

	for detector in detectors:
		detector_params=[]
		if detector == "aid":
			detector_params = aid_thr
		elif detector == "mad":
			detector_params = mad_thr
		elif detector == "orth": 
                        detector_params = orth_thr
		elif detector == "newsum": 
                        detector_params = newsum_thr

		for iparam in range(0,len(detector_params)):
			for rifile in range(0, len(ready_for_execution_input),num_included):
				sbatchfname=detector + "_" + detector_params[iparam] + "_sbatch_" + str(fid)
				fid = fid + 1
				#print sbatchfname
			
				fsbatch = open(sbatchdir + sbatchfname + ".sh", "w")
				fsbatch.write("#!/bin/bash\n")
				fsbatch.write("#SBATCH --partition=pal\n")
				fsbatch.write("#SBATCH --account pal\n")
				fsbatch.write("#SBATCH -N 1\n")
				fsbatch.write("#SBATCH -e "+ sbatchfname + ".err\n")
				fsbatch.write("#SBATCH -o /dev/null\n")

				start_point=rifile;
				end_point=rifile + num_included
				if end_point > len(ready_for_execution_input):
					end_point = len(ready_for_execution_input)
					#print end_point
				
				#print start_point
				#print end_point
				for index in range(start_point, end_point):
					farray = ready_for_execution_output[index].split("_")
					solver = farray[0]
					dataset = farray[1]
					if dataset == "af" or dataset == "Pres":
						dataset= dataset + "_" + farray[2]

					ofile = simulationdir + detector + "/" + detector_params[iparam] + "/" + solver + "/"+ dataset + "/" + ready_for_execution_output[index] + ".out"
					#print ofile
					if detector == "orth":
						if solver == "cgs" or solver == "bicgsta":
							continue

					cline = "python static_detect.py " + detector + " " + ready_for_execution_input[index] + " "
					if detector == "mad": #need to specify window size
						#print ready_for_execution_output[index]
						wsize = find_mad_wsize(ready_for_execution_output[index])
						#print wsize
						cline = cline + wsize + " "
					cline = cline + detector_params[iparam]  + " > " + ofile + " &\n"

					fsbatch.write("if [ ! -f " + ofile + " ]\n")
					fsbatch.write("then\n")
					fsbatch.write("\t" + cline)
					fsbatch.write("elif [ \"$( grep -re \"Last iteration found\" "+ ofile + " | wc -l )\" == \"0\" ] \n")
					fsbatch.write("then\n")
					fsbatch.write("\t" + cline)
					fsbatch.write("fi\n\n")

				fsbatch.write("wait\n")
				fsbatch.close()

def generate_sbatchs():
	all_ready_for_execution_input=[]
        all_ready_for_execution_output=[]

        cg_ready_for_execution_input=[]
	cg_ready_for_execution_output=[]

        cgs_ready_for_execution_input=[]
	cgs_ready_for_execution_output=[]

        iccg_ready_for_execution_input=[]
	iccg_ready_for_execution_output=[]

        bicg_ready_for_execution_input=[]
	bicg_ready_for_execution_output=[]

        bicgsta_ready_for_execution_input=[]
	bicgsta_ready_for_execution_output=[]

	for ex in all_files:
		info=parse_data(ex)
		if info['ready'] == 1:
			if info['solver'] == 'cg':
				cg_ready_for_execution_input.append(ex)
				cg_ready_for_execution_output.append(info['ofname'])
			elif info['solver'] == 'cgs':
				cgs_ready_for_execution_input.append(ex)
				cgs_ready_for_execution_output.append(info['ofname'])
			elif info['solver'] == 'iccg':
                                iccg_ready_for_execution_input.append(ex)
				iccg_ready_for_execution_output.append(info['ofname'])
			elif info['solver'] == 'bicg':
                                bicg_ready_for_execution_input.append(ex)
				bicg_ready_for_execution_output.append(info['ofname'])
			elif info['solver'] == 'bicgsta':
                                bicgsta_ready_for_execution_input.append(ex)
				bicgsta_ready_for_execution_output.append(info['ofname'])

			all_ready_for_execution_input.append(ex)
			all_ready_for_execution_output.append(info['ofname'])
		else:
			missing_files.append(ex)
			
	print "DETECTORS: Writing to files starting..., size(ready_files): ", len(all_ready_for_execution_input)
	#write_to_file_for_detectors(all_ready_for_execution_input, all_ready_for_execution_output)
	print "DETECTORS: Writing to files ending..., size(ready_files): ", len(all_ready_for_execution_input)

	print "ML-CG: Writing to files starting..., size(ready_files): ", len(cg_ready_for_execution_input)
        write_to_file_for_ml('cg',cg_ready_for_execution_input,cg_ready_for_execution_output)
        print "ML-CG: Writing to files ending..., size(ready_files): ", len(cg_ready_for_execution_input)

	print "ML-CGS: Writing to files starting..., size(ready_files): ", len(cgs_ready_for_execution_input)
	write_to_file_for_ml('cgs',cgs_ready_for_execution_input, cgs_ready_for_execution_output)
        print "ML-CGS: Writing to files ending..., size(ready_files): ", len(cgs_ready_for_execution_input)

	print "ML-ICCG: Writing to files starting..., size(ready_files): ", len(iccg_ready_for_execution_input)
	write_to_file_for_ml('iccg',iccg_ready_for_execution_input, iccg_ready_for_execution_output)
        print "ML-ICCG: Writing to files ending..., size(ready_files): ", len(iccg_ready_for_execution_input)

	print "ML-BICG: Writing to files starting..., size(ready_files): ", len(bicg_ready_for_execution_input)
	write_to_file_for_ml('bicg',bicg_ready_for_execution_input, bicg_ready_for_execution_output)
        print "ML-BICG: Writing to files ending..., size(ready_files): ", len(bicg_ready_for_execution_input)

	print "ML-BICCGSTA: Writing to files starting..., size(ready_files): ", len(bicgsta_ready_for_execution_input)
	write_to_file_for_ml('bicgsta', bicgsta_ready_for_execution_input, bicgsta_ready_for_execution_output)
        print "ML-BICCGSTA: Writing to files ending..., size(ready_files): ", len(bicgsta_ready_for_execution_input)

	rvalue=len(all_ready_for_execution_input)
	return rvalue

def main():
	#global num_all_files, num_completed_files, all_files, all_ready_for_execution_output, missing_files, all_ready_for_execution_input, fid
	global num_all_files, num_completed_files, all_files, missing_files, fid
	
	fid = 0
	num_all_files=0
	num_completed_files=0
	all_files=[]
	#all_ready_for_execution_output=[]
	missing_files=[]
	#all_ready_for_execution_input=[]

        for rpath, rsubdirs, rfiles in os.walk(datadir):
                for spath, ssubdirs, sfiles in os.walk(rpath):
                        for dfile in sfiles:
				if "beta" in dfile:
					#print "Beta is not included"
					continue
                                fname=spath + "/" + dfile                       
				#info=parse_data(fname)
                                #ofname=info['ofname']
                                #farray = ofname.split("_")
                                #solver = farray[0]
                                #if solver in solvers:
                                 #       print "Adding file: ", fname
				all_files.append(fname)
				if len(all_files) == max_limit:
					num_all_files = num_all_files + len(all_files)
					print "Generation of sbatches starting...stage 1"
					rvalue=generate_sbatchs() 
					print "Generation of sbatches ending...stage 1"
					num_completed_files= num_completed_files + rvalue
					del all_files[:]
					
	if len(all_files) > 0:
		num_all_files = num_all_files + len(all_files)
		print "Generation of sbatches starting...stage 2"
		rvalue=generate_sbatchs()
		print "Generation of sbatches ending...stage 2"
		num_completed_files= num_completed_files + rvalue

        dump_output()

main()
			

		
			



	
        

