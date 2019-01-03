import sys
from sys import argv
from random import randint
from random import shuffle
import os
import glob, os

aid = "aid"
baseline = "baseline"
ssd1 = "ssd1"
newsum = "newsum"
gagan = "gagan"
orth = "orth"
detectors= ["aid","gagan", "newsum", "orth", "ssd1"]
detectors_text = ["Bit", "detected", "detected", "detected", "detected"]
solvers = ["cg","cgs", "iccg", "bicg","bicgsta", "qmr"]
aidparams= ["0.00001", "0.000001", "0.001", "0.00001", "0.000001", "0.00078125"]
ssdparams=["0.000001", "0.000001", "0.01", "0.000001", "0.000001", "0.000001"]
gaganparams = ["0.1", "0.1", "0.1", "0.1", "0.1", "0.1"]
newsumparams = ["1e-09", "1e-010", "1e-010", "1e-09", "1e-010", "1e-010"]
newsumparamStr= ["0.000000001", "0.0000000001", "0.0000000001", "0.000000001", "0.0000000001", "0.0000000001"]
orthparams = ["1e-010", "1e-010", "1e-010", "1e-010", "1e-010", "1e-010"]
orthparamStr = ["0.0000000001", "0.0000000001", "0.0000000001", "0.0000000001", "0.0000000001", "0.0000000001"]

#fault injection
bfrange = range(0,64)
inj = [1,2,3]
magnitude = [0.2, 0.4, 0.6, 0.8]
datasets = ['af_shell3', 'af_shell4', 'af_shell7', 'af_shell8', 'bcsstk13','bcsstk14', 'bcsstk15', 'bcsstk16', 'bcsstk24', 'bcsstk27', 'bcsstk28', 'ex3', 'ex9', 'ex13', 'ex15', 'Kuu', 'msc04515', 'bcsstk38', 'nasa2146', 'Pres_Poisson', 'sts4098', 's1rmq4m1', 's1rmt3m1', 's2rmq4m1', 's2rmt3m1', 's3rmq4m1', 's3rmt3m1','s3rmt3m3']

fault_type="memory"
FAULTY_LABEL="1 "                                                                                                      
NON_FAULTY_LABEL="-1 "  


def getChunks(fname, chunksize, blabel):
	residuals = []
	tmpresiduals = []
	injPlace = 0
	features=[]
	
	try:
		with open(fname,'r') as fl:
			lines = fl.readlines()

			for x in lines:
				if ("Random" in x):
					raise SystemExit
				if("Inserted" in x):
					temp = x.split()
					injPlace = int(temp[len(temp)-1])
                                        #print injPlace
				if("RESID" in x and "VALIDATION" not in x):
					if(("nan" in x) or ("inf" in x)):
						residuals.append("1")
					elif(len(x)>15):
						residuals.append("1")
					else:
						residuals.append(x[6:len(x)-1])
	except IOError:
		msg = "File does not exist in getChunks=" + fname
		print msg
		raise SystemExit

	if(len(residuals) < int(chunksize)):
		msg = "Not enough residual numbers solver= " +solvers[sindex] + " dataset= " + nm
		print msg
		return features
	
	chunkNum = len(residuals) / int(chunksize)
	for chunkIndex in range(0,chunkNum):
		numfeatures = 1
		tmpresiduals.append("")
		features.append("")
		ilabel=""
		for x in residuals[chunkIndex*int(chunksize):chunkIndex*int(chunksize)+int(chunksize)]:
			tmpresiduals[chunkIndex] = tmpresiduals[chunkIndex] + str(numfeatures) + ":" + x + " "
			numfeatures = numfeatures + 1
		if(injPlace < chunkIndex*int(chunksize)+int(chunksize)):
			ilabel = blabel
		else:
			ilabel = "-1 "
		features[chunkIndex] = ilabel + tmpresiduals[chunkIndex]
	
	return features

def generateSets(fset_file):
	features = []
        count = 0
        for fname in fset_file:
                baseline_fname= results_dir + "baseline/" + solver + "/" + solver + "_baseline_" + fname + ".out"
                try:
			with open(baseline_fname,'r') as fl:
                                if 'PASSED' in fl.read():
                                        flabel = NON_FAULTY_LABEL
                                else: flabel = FAULTY_LABEL
				
                                svm_fname = results_dir + "svm/" + solver + "/" + solver + "_svm_" + fname + ".out"
                                rfeatures = getChunks(svm_fname, chunksize, flabel)
                                features=features + rfeatures
				
                                msg = "Files id= " + str(count) + " " + str(len(rfeatures)) + " " + str(len(features))
                                print msg

		except IOError:
                        msg = "File does not exist in generateSets=" + baseline_fname
                        print msg
                        raise SystemExit

                count =  count + 1

	return features


if len(argv) < 7:
	print("Usage: SOLVER #CHUNKSIZE RESULT_DIR DATASET_NUMBER TRAINING_SIZE TESTING_SIZE", (argv[0]))
	sys.exit(1)
	
solver = argv[1]
sindex = solvers.index(solver)
chunksize = argv[2]
results_dir=argv[3]
dataset_num = int(argv[4])
training_size = int(argv[5])
total_training_size = training_size + int(training_size * 0.1)
testing_size = int(argv[6])


def main():
	current_dir = dir_path = os.path.dirname(os.path.realpath(__file__))
	output_dir = current_dir + "/data/datasets/"+fault_type+"/anomaly/"+solver+"/"+str(dataset_num) + "_" + str(chunksize) + "_" + str(training_size) + "_" + str(testing_size) + "/"
	
	if not os.path.exists(output_dir):
		os.makedirs(output_dir)

	################### TRAINING #######################
	count = 0
	lst_training = []
	training_datasets = []
	while(count < dataset_num):
		r1 = randint(0, len(datasets))
		if(r1 not in lst_training):
			lst_training.append(r1)
			count = count + 1

	for i in range(0, len(datasets)):
		if(i in lst_training):
			training_datasets.append(datasets[i])

			
	files = []
        for i in inj:
                for j in magnitude:
                        for k in bfrange:
                                for d in training_datasets:
                                        fname=str(i) + "_" + str(j) + "_" + str(k) + "_" + d
                                        files.append(fname)

        fset_file = []
        if (len(files) > total_training_size):
                shuffle(files)
                shuffle(files)
                fset_file=files[:total_training_size]
        else:
                fset_file = files

	training_features = generateSets(fset_file)
	shuffle(training_features)
        training_set=training_features[0:training_size]      
	testing_on_set=training_features[training_size+1:total_training_size]
 	#print len(training_set)


	training_set_fname = output_dir + str(dataset_num) + "_" + str(chunksize) + "_" + str(training_size) + "_" + str(testing_size) + ".train"
	with open(training_set_fname,'w') as ftr:
		for line in training_set:
			#print line
			ftr.write(line + '\n')

	ftr.close()

	testing_on_fname = output_dir + str(dataset_num) + "_" + str(chunksize) + "_" + str(training_size) + "_" + str(testing_size) + ".test"
        with open(testing_on_fname,'w') as ftron:
                for line in testing_on_set:
                        ftron.write(line + '\n')

        ftron.close()


	################## TESTING #######################
	testing_files = []
	for i in inj:
		for j in magnitude:
			for k in bfrange:
				for d in datasets:
					fname=str(i) + "_" + str(j) + "_" + str(k) + "_" + d
					testing_files.append(fname)

	ftesting_files = []
	shuffle(testing_files)
	shuffle(testing_files)
	ftesting_files=testing_files[:testing_size]

        previous_testing_files = glob.glob(output_dir + "/"+ solver+"_*.test")
	for rf in previous_testing_files:
		os.remove(rf)

	tarray_form=[]
	tarray_form.append("")
	for testing_file in ftesting_files:
		otesting_file =  output_dir + solver + "_" + testing_file + ".test"
		
		#generateSets excepts a list
		tarray_form[0] = testing_file
		
		testing_set = generateSets(tarray_form)
		with open(otesting_file,'w') as fts:
			for line in testing_set:
				fts.write(line + '\n')
	
	print "Completed!"

main()
