from __future__ import division
import os
from random import shuffle
import sys
from sys import argv


numsamples= ["1000", "2000", "4000", "6000", "8000", "10000", "12000", "14000", "16000"]
#numsamples= ["1000"]
threshold=0.1
signBit=range(63,64)
mantissaBits=range(0,52)
exponentBits=range(52,63)

def writeToFile(fname,data,start,end):
	with open(fname,'w') as fp:
		if len(data) > 0:
			for line in data[start:end]:
				features = line.split()
				fp.write(" ".join(str(x) for x in features[0:len(features)-1])+"\n")
		else:
			fp.write("\n")
	fp.close()

def getSubset(data, start, end, crange):
	subset= []
	for row in data[start:end]:
		srow = row.split()
		if int(srow[len(srow)-1]) in crange:
			subset.append(row)
	return subset

def genTrainingTesting(data, train_dir, app, iclass, tsize, training_start, training_end, testing_start, testing_end):
	#print str(training_start)+" "+str(training_end)
	#print str(testing_start)+" "+str(testing_end)
	otrain=train_dir+app+"_train_"+iclass+"_"+tsize+".dat"
        writeToFile(otrain,data,training_start,training_end)

        otest=test_dir+app+"_test_"+iclass+"_all_"+tsize+".dat"
        writeToFile(otest,data,testing_start,testing_end)

        otest=test_dir+app+"_test_"+iclass+"_sign_"+tsize+".dat"
        sign= getSubset(data,testing_start,testing_end,signBit)
        writeToFile(otest,sign,0,len(sign))

        otest=test_dir+app+"_test_"+iclass+"_mantissa_"+tsize+".dat"
        mantissa= getSubset(data,testing_start,testing_end,mantissaBits)
        writeToFile(otest,mantissa,0,len(mantissa))

        otest=test_dir+app+"_test_"+iclass+"_exponent_"+tsize+".dat"
        exponent= getSubset(data,testing_start,testing_end,exponentBits)
        writeToFile(otest,exponent,0,len(exponent))

def dual(data, app):
	rlst = []
	for row in data:
                srow = row.split()
		srow[0] = str(int(srow[0]) * (-1))
		if len(srow) > 12: #defitions from the detectors are here
			srow[11] = srow[11].split(":")[0]+":"+str(int(srow[11].split(":")[1]) * (-1))
			srow[12] = srow[12].split(":")[0]+":"+str(int(srow[12].split(":")[1]) * (-1))
			srow[13] = srow[13].split(":")[0]+":"+str(int(srow[13].split(":")[1]) * (-1))
			srow[14] = srow[14].split(":")[0]+":"+str(int(srow[14].split(":")[1]) * (-1))
			if app != "cgs" and app != "bicgsta" and app != "qmr":
				srow[15] = srow[15].split(":")[0]+":"+str(int(srow[15].split(":")[1]) * (-1))

		rlst.append(" ".join(str(x) for x in srow)+"\n")	

	return rlst

if len(argv) < 2:
        print("Usage: Idataset", (argv[0]))
        sys.exit(1)
else:
        idataset= argv[1]
	fname=idataset.split("/")
	print fname

	sapp=str(fname[4]).split(".dat")
	app=sapp[0]
	appname=app.split("_")[0]
	print app
	print appname

train_dir=fname[0]+"/"+fname[1]+"/"+fname[2]+"/"+fname[3]+"/training/"+appname+"/"
test_dir=fname[0]+"/"+fname[1]+"/"+fname[2]+"/"+fname[3]+"/testing/"+appname+"/"
print train_dir
print test_dir




for tsize in numsamples:
	lst = []
	with open(idataset,'r') as ds:
		for line in ds:
			lst.append(line)
                                
	ds.close()

	#shuffle the examples in the dataset
	shuffle(lst)
	shuffle(lst)

	#dual of dataset
	dual_lst=dual(lst,appname)
	
        ######################TRAINING 1##########################
	training_start = 0
	training_end = training_start + int(tsize)
	testing_start = training_end + 1
	testing_end = testing_start + int(round(float(tsize) * float(threshold)))
	
	genTrainingTesting(lst, train_dir, app, "1_normal", tsize, training_start,training_end,testing_start,testing_end)
	genTrainingTesting(dual_lst, train_dir, app, "1_dual", tsize, training_start,training_end,testing_start,testing_end)


	#########################TRAINING 2#########################
	training_start = 0
	training_end = testing_end
	testing_start = training_end + 1
	testing_end = len(lst) 

        genTrainingTesting(lst, train_dir, app, "2_normal", tsize, training_start, training_end, testing_start, testing_end)
	genTrainingTesting(dual_lst, train_dir, app, "2_dual", tsize, training_start, training_end, testing_start, testing_end)




	
        

