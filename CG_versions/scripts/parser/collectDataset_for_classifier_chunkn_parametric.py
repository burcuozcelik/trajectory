import sys
from sys import argv
from random import randint
from random import shuffle
aid = "aid"
baseline = "baseline"
ssd1 = "ssd1"
newsum = "newsum"
gagan = "gagan"
orth = "orth"


detectors= ["aid","gagan", "newsum", "orth", "ssd1"]
detectors_text = ["Bit", "detected", "detected", "detected", "detected"]

solvers = ["cg","cgs", "iccg", "bicg","bicgsta", "qmr"]
#aidparams = "0.00078125" #"0.01","0.001","0.0001","0.00001","0.000001",
#ssdparams = "0.00078125"#"0.01","0.001","0.0001","0.00001","0.000001",
#newsumparams = "1e-08"#"1e-06","1e-07","1e-08","1e-09","1e-010"                                                                                                          
#newsumparamStr = "0.00000001"#"0.000001","0.0000001","0.00000001","0.000000001","0.0000000001"    
#orthparams = "1e-08"#"1e-06","1e-07","1e-08","1e-09","1e-010"                                                                                                            
#orthparamStr = "0.00000001"#"0.000001","0.0000001","0.00000001","0.000000001","0.0000000001" 

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

datasets1 = ['ex3', 'ex15', 'bcsstk38']

num_FI = len(bfrange) * len(inj) * len(magnitude)

if len(argv) < 7:
        print("Usage: SOLVER #CHUNKSIZE RESULT_DIR DATASET_NUMBER #FI INDEX", (argv[0]))
        sys.exit(1)

solver = argv[1]
sindex = solvers.index(solver)
print sindex
res_num = argv[2]
results_dir=argv[3]
dataset_num = int(argv[4])
numFI = int(argv[5])
index2 = int(argv[6])

fault_type="memory"
training_dataset = []
test_dataset = []
lst_training = []
count = 0

while(count < dataset_num):
	r1 = randint(0, len(datasets))
	if(r1 not in lst_training):
		lst_training.append(r1)
		count = count + 1

for i in range(0, len(datasets)):
	if(i in lst_training):
		training_dataset.append(datasets[i])
	else:
		test_dataset.append(datasets[i])


lst_FI = []
count = 0
while(count < numFI):
	r1 = randint(0, num_FI)
	if(r1 not in lst_FI):
		lst_FI.append(r1)
		count = count + 1


FAULTY_LABEL="1 "
NON_FAULTY_LABEL="-1 "

if True:
    train2_w="./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_wDetectors.train2"
    train2_wo="./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_woDetectors.train2"
    cnt = 0
    with open(train2_w,'w') as ftrain2_w:
        with open(train2_wo,'w') as ftrain2_wo:
            for i in inj:
                for j in magnitude:
                    for k in bfrange:
                        if(cnt in lst_FI):
                            for nm in training_dataset:
                                dirbaseline = results_dir + baseline + "/" + solver + "/"
                                features=[]
                                flabel=""
                                resdir = results_dir + "/svm/"
                                
                                try:
                                    with open(dirbaseline + solver+ "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out",'r') as fl:
                                        if 'PASSED' in fl.read():
                                            flabel = NON_FAULTY_LABEL
                                        else: flabel = FAULTY_LABEL

                                        residuals = []
                                        tmpresiduals = []
                                        injPlace = 0
                                        fsolver=resdir + "/" + solver + "/"  +solver+"_"+"svm"  +"_"+ str(i)+"_" + str(j) +"_" + str(k) +"_" + nm+ ".out"
                                        with open(fsolver,'r') as fl2:
                                            lines = fl2.readlines()
                                            
                                            for x in lines:
                                                if ("Random" in x):
                                                    raise SystemExit
                                                if("Inserted" in x):
                                                    #print fsolver
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
                                                                    
                                            if(len(residuals) < int(res_num)):
                                                    msg = "Not enough residual numbers solver= " +solver + " dataset= " + nm
                                                    print msg
                                                    continue

                                            chunkNum = len(residuals) / int(res_num)
                                            
                                            for chunkIndex in range(0,chunkNum):
                                                numfeatures = 1
                                                tmpresiduals.append("")
                                                features.append("")
                                                ilabel=""
                                                for x in residuals[chunkIndex*int(res_num):chunkIndex*int(res_num)+int(res_num)]:
                                                        tmpresiduals[chunkIndex] = tmpresiduals[chunkIndex] + str(numfeatures) + ":" + x + " "
                                                        numfeatures = numfeatures + 1
                                                if(injPlace < chunkIndex*int(res_num)+int(res_num)):
                                                    ilabel = flabel
                                                else: 
                                                    ilabel = "-1 "
                                                features[chunkIndex] = ilabel + tmpresiduals[chunkIndex]
                                                ftrain2_wo.write(features[chunkIndex] + '\n')

                                except IOError: 
                                        msg = "Train-2 File does not exist =" + dirbaseline + solver+ "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out"
                                        print msg
                                        raise SystemExit
                                        

                                numfeatures = int(res_num) + 1

                                if True:
                                        for index in range(0,len(detectors)):
                                                if detectors[index] == "aid":
                                                        params = aidparams[sindex]
                                                        paramstr = aidparams[sindex]
                                                elif detectors[index] == "gagan":
                                                        params = gaganparams[sindex]
                                                        paramstr = gaganparams[sindex]
                                                elif detectors[index] == "newsum":
                                                        params = newsumparams[sindex]
                                                        paramstr = newsumparamStr[sindex]
                                                elif detectors[index] == "orth":
                                                        params = orthparams[sindex]
                                                        paramstr = orthparamStr[sindex]
                                                elif detectors[index] == "ssd1":
                                                        params = ssdparams[sindex]
                                                        paramstr = ssdparams[sindex]

                                                try:
                                                        with open(results_dir+detectors[index]+"/"+solver+"/"+params+"/"+solver+"_"+detectors[index]+"_"+paramstr+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out",'r') as fl:
                                                
                                                                for chunkIndex in range(0,chunkNum):
                                                                        if (detectors_text[index] in fl.read()):
                                                                                features[chunkIndex] = features[chunkIndex] + str(numfeatures) + ":"+ FAULTY_LABEL
                                                                        else: features[chunkIndex] = features[chunkIndex] + str(numfeatures) + ":" + NON_FAULTY_LABEL
                                                                numfeatures = numfeatures +1
                                                except IOError:
                                                        msg = "Train-2 File does not exist=" + results_dir+detectors[index]+"/"+solver+"/"+params+"/"+solver+"_"+detectors[index]+"_"+paramstr+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out"
                                                        print msg
                                                        if detectors[index] == "orth" and solver == "cgs" or solver == "bicgsta" or solver == "qmr":
                                                                continue
                                                        else:
                                                                raise SystemExit



                                for chunkIndex in range(0,chunkNum):
                                        features[chunkIndex] = features[chunkIndex];
                                        ftrain2_w.write(features[chunkIndex] + '\n')
                        cnt = cnt + 1
        ftrain2_wo.close()
    ftrain2_w.close()

    print "Done Training-2 generated!!!"

    train1_ratio=0.9

    with open("./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_woDetectors.train1",'w') as train1_wo:
        with open("./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_wDetectors.train1",'w') as train1_w:
		with open("./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_woDetectors.test1",'w') as test1_wo:
        		with open("./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_wDetectors.test1",'w') as test1_w:
				fname="./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_wDetectors.train2"
       	 			with open(fname,'r') as flst:
					lines = flst.readlines()
					shuffle(lines)
					shuffle(lines)
					
					length = 9*len(lines)/10
				
					for line in lines[0:length]:
						temp = line.split()
						line_wo = ""
						for x in range(0,int(res_num)+1):
							line_wo = line_wo  + temp[x] +  " "
						train1_wo.write(line_wo+"\n")
						
					for line in lines[0:length]:
						train1_w.write(line)
						
					for line in lines[length+1:]:
						temp = line.split()
						line_wo = ""
						for x in range(0,int(res_num)+1):
							line_wo = line_wo +temp[x] +  " "
						test1_wo.write(line_wo+"\n")
						
					for line in lines[length+1:]:
						test1_w.write(line)
				flst.close()		
			train1_wo.close()
		train1_w.close()
	test1_wo.close()
    test1_w.close()

print "Done Training-1 and testing-1 generated"

if True:
    #print fname, fname2
    cnt = 0
    for i in inj:
    	for j in magnitude:
        	for k in bfrange:
                	if(cnt not in lst_FI):
                            for nm in datasets:				               
				    fname="./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_wDetectors_"+nm+"_"+str(i)+"_"+str(j)+"_"+str(k)+".test2"
    				    fname2="./data/datasets/"+fault_type+"/"+solver+"/"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"/"+solver+"_"+argv[2]+"_"+argv[4]+"_"+argv[5]+"_"+argv[6]+"_woDetectors_"+nm+"_"+str(i)+"_"+str(j)+"_"+str(k)+".test2"
				    with open(fname2,'w') as test2_wo:
        				with open(fname,'w') as test2_w:
				                dirbaseline = results_dir + baseline + "/" + solver + "/"
				                features=[]
				                flabel=""
				                resdir = results_dir + "/svm/"

				                try:
				                    with open(dirbaseline + solver+ "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out",'r') as fl:
				                        if 'PASSED' in fl.read():
				                            flabel = NON_FAULTY_LABEL
				                        else: flabel = FAULTY_LABEL

				                        residuals = []
				                        tmpresiduals = []
				                        injPlace = 0
				                        fsolver=resdir + "/" + solver + "/"  +solver+"_"+"svm"  +"_"+ str(i)+"_" + str(j) +"_" + str(k) +"_" + nm+ ".out"
				                        with open(fsolver,'r') as fl2:
				                            lines = fl2.readlines()
				                            
				                            for x in lines:
				                                if ("Random" in x):
				                                    raise SystemExit
				                                if("Inserted" in x):
				                                    print fsolver
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
				                                                    
				                            if(len(residuals) < int(res_num)):
				                                    msg = "Not enough residual numbers solver= " +solver + " dataset= " + nm
				                                    print msg
				                                    continue

				                            chunkNum = len(residuals) / int(res_num)
				                            
				                            for chunkIndex in range(0,chunkNum):
				                                numfeatures = 1
				                                tmpresiduals.append("")
				                                features.append("")
				                                ilabel=""
				                                for x in residuals[chunkIndex*int(res_num):chunkIndex*int(res_num)+int(res_num)]:
				                                        tmpresiduals[chunkIndex] = tmpresiduals[chunkIndex] + str(numfeatures) + ":" + x + " "
				                                        numfeatures = numfeatures + 1
				                                if(injPlace < chunkIndex*int(res_num)+int(res_num)):
				                                    ilabel = flabel
				                                else: 
				                                    ilabel = "-1 "
				                                features[chunkIndex] = ilabel + tmpresiduals[chunkIndex]
				                                #print features[chunkIndex]
				                                test2_wo.write(features[chunkIndex] + '\n')
								

				                except IOError: 
				                        msg = "Test-2 File does not exist =" + dirbaseline + solver+ "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out"
				                        print msg
				                        raise SystemExit
				                        

				                numfeatures = int(res_num) + 1

				                if True:#wDetectors == "1":
				                        for index in range(0,len(detectors)):
				                                if detectors[index] == "aid":
				                                        params = aidparams[sindex]
				                                        paramstr = aidparams[sindex]
				                                elif detectors[index] == "gagan":
				                                        params = gaganparams[sindex]
				                                        paramstr = gaganparams[sindex]
				                                elif detectors[index] == "newsum":
				                                        params = newsumparams[sindex]
				                                        paramstr = newsumparamStr[sindex]
				                                elif detectors[index] == "orth":
				                                        params = orthparams[sindex]
				                                        paramstr = orthparamStr[sindex]
				                                elif detectors[index] == "ssd1":
				                                        params = ssdparams[sindex]
				                                        paramstr = ssdparams[sindex]

				                                try:
				                                        with open(results_dir+detectors[index]+"/"+solver+"/"+params+"/"+solver+"_"+detectors[index]+"_"+paramstr+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out",'r') as fl:
				                                
				                                                for chunkIndex in range(0,chunkNum):
				                                                        if (detectors_text[index] in fl.read()):
				                                                                features[chunkIndex] = features[chunkIndex] + str(numfeatures) + ":"+ FAULTY_LABEL
				                                                        else: features[chunkIndex] = features[chunkIndex] + str(numfeatures) + ":" + NON_FAULTY_LABEL
				                                                numfeatures = numfeatures +1
				                                except IOError:
				                                        msg = "Test-2 File does not exist=" + results_dir+detectors[index]+"/"+solver+"/"+params+"/"+solver+"_"+detectors[index]+"_"+paramstr+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out"
				                                        print msg
				                                        if detectors[index] == "orth" and solver == "cgs" or solver == "bicgsta" or solver == "qmr":
				                                                continue
				                                        else:
				                                                raise SystemExit



				                for chunkIndex in range(0,chunkNum):
				                        features[chunkIndex] = features[chunkIndex];
				                        test2_w.write(features[chunkIndex] + '\n')
				    	test2_w.close()
			    test2_wo.close()
			cnt = cnt + 1
