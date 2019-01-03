import sys
from sys import argv


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

datasets = ['af_shell3', 'af_shell4', 'af_shell7', 'af_shell8', 'aft01', 'bcsstk13','bcsstk14', 'bcsstk15', 'bcsstk16', 'bcsstk24', 'bcsstk27', 'bcsstk28', 'crystm01', 'ex3', 'ex9', 'ex13', 'ex15', 'Kuu', 'msc04515', 'Muu', 'bcsstk38', 'nasa2146', 'Pres_Poisson', 'sts4098', 's1rmq4m1', 's1rmt3m1', 's2rmq4m1', 's2rmt3m1', 's3rmq4m1', 's3rmt3m1','s3rmt3m3']


if len(argv) < 4:
        print("Usage: #CHUNKSIZE RESULT_DIR wDETECTOR", (argv[0]))
        sys.exit(1)

res_num = argv[1]
results_dir=argv[2]
wDetectors=argv[3]


FAULTY_LABEL="1 "
NON_FAULTY_LABEL="-1 "

for sindex in range(0,len(solvers)):
   if wDetectors == "1":
           fname="./data/datasets/"+solvers[sindex]+"_chunk"+res_num+"_wDetectors.dat"
   else:
           fname="./data/datasets/"+solvers[sindex]+"_chunk"+res_num+"_woDetectors.dat"
   print fname

   with open(fname,'w') as fls:

      for i in inj:
         for j in magnitude:
            for k in bfrange:
               for nm in datasets:
                  dirbaseline = results_dir + baseline + "/" + solvers[sindex] + "/"
                  features=[]
		  flabel=""
		  resdir = results_dir + "/svm/"
                  

		  try:
                    with open(dirbaseline + solvers[sindex] + "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out",'r') as fl:
                        if 'PASSED' in fl.read():
                              flabel = NON_FAULTY_LABEL
                        else: flabel = FAULTY_LABEL

			residuals = []
			tmpresiduals = []
                        injPlace = 0
			fsolver=resdir + "/" + solvers[sindex] + "/"  +solvers[sindex]+"_"+"svm"  +"_"+ str(i)+"_" + str(j) +"_" + str(k) +"_" + nm+ ".out"
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
				msg = "Not enough residual numbers solver= " +solvers[sindex] + " dataset= " + nm
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

                  except IOError: 
                        msg = "File does not exist =" + dirbaseline + solvers[sindex] + "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out"
                        print msg
			raise SystemExit
                         

                  numfeatures = int(res_num) + 1

		  if wDetectors == "1":
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
					  with open(results_dir+detectors[index]+"/"+solvers[sindex]+"/"+params+"/"+solvers[sindex]+"_"+detectors[index]+"_"+paramstr+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out",'r') as fl:
                                   
						  for chunkIndex in range(0,chunkNum):
							  if (detectors_text[index] in fl.read()):
								  features[chunkIndex] = features[chunkIndex] + str(numfeatures) + ":"+ FAULTY_LABEL
							  else: features[chunkIndex] = features[chunkIndex] + str(numfeatures) + ":" + NON_FAULTY_LABEL
						  numfeatures = numfeatures +1
				  except IOError:
					  msg = "File does not exist=" + results_dir+detectors[index]+"/"+solvers[sindex]+"/"+params+"/"+solvers[sindex]+"_"+detectors[index]+"_"+paramstr+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out"
					  print msg
					  if detectors[index] == "orth" and solvers[sindex] == "cgs" or solvers[sindex] == "bicgsta" or solvers[sindex] == "qmr":
                                                  continue
                                          else:
                                                  raise SystemExit



                  for chunkIndex in range(0,chunkNum):
                        features[chunkIndex] = features[chunkIndex] + str(k);
                        fls.write(features[chunkIndex] + '\n')
		  

   fls.close()
