from __future__ import division
import sys
from sys import argv

aid = "aid"
baseline = "baseline"
ssd1 = "ssd1"
newsum = "newsum"
gagan = "gagan"
orth = "orth"

#solvers = ["cg", "cgs", "iccg", "bicg", "bicgsta", "qmr"] 
solvers = ["cg"]  
#detectors= ["aid","gagan", "newsum", "orth", "ssd1"]
detectors= ["aid"]  
detectors_text = ["Bit", "detected", "detected", "detected", "detected"]

aidparams = ["0.00078125", "0.01","0.001","0.0001","0.00001","0.000001"]
ssdparams = ["0.00078125","0.01","0.001","0.0001","0.00001","0.000001"]
gaganparams = ["0.1"]

newsumparams = ["1e-08","1e-06","1e-07","1e-08","1e-09","1e-010"]
newsumparamStr = ["0.00000001","0.000001","0.0000001","0.00000001","0.000000001","0.0000000001"]

orthparams = ["1e-08","1e-06","1e-07","1e-08","1e-09","1e-010"]              
orthparamStr = ["0.00000001","0.000001","0.0000001","0.00000001","0.000000001","0.0000000001"]

#fault injection
bfrange_all = range(0,64)
bfrange_sign= range(63,64)
bfrange_exponent=range(52,63)
bfrange_mantissa=range(0,52)

inj = [1,2,3]
magnitude = [0.2, 0.4, 0.6, 0.8]

datasets = ['af_shell3', 'af_shell4', 'af_shell7', 'af_shell8', 'bcsstk13','bcsstk14', 'bcsstk15', 'bcsstk16', 'bcsstk24', 'bcsstk27', 'bcsstk28', 'ex3', 'ex9', 'ex13', 'ex15', 'Kuu', 'msc04515', 'bcsstk38', 'nasa2146', 'Pres_Poisson', 'sts4098', 's1rmq4m1', 's1rmt3m1', 's2rmq4m1', 's2rmt3m1', 's3rmq4m1', 's3rmt3m1','s3rmt3m3']


if len(argv) < 3:
        print("Usage: RESULT_DIR bit-flip-type", (argv[0]))
        sys.exit(1)

results_dir=argv[1]
bftype=argv[2]

if bftype == "all":
	bfrange = bfrange_all
elif bftype == "sign":
	bfrange = bfrange_sign
elif bftype == "exponent":
	bfrange = bfrange_exponent
elif bftype == "mantissa":
	bfrange = bfrange_mantissa

 
for solver in solvers:
   for index in range(0,len(detectors)):
     detector=detectors[index]
     if detector == "aid":
        params = aidparams
	paramstr = aidparams
     elif detector == "gagan":
        params = gaganparams
	paramstr = gaganparams
     elif detector == "newsum":
        params = newsumparams
	paramstr = newsumparamStr
     elif detector == "orth":
        params = orthparams
	paramstr = orthparamStr
     elif detector == "ssd1":
        params = ssdparams
	paramstr = ssdparams

     TP=TN=FP=FN=0
     for pindex in range(0,len(params)): 
       TP=TN=FP=FN=total=0
       for i in inj:
          for j in magnitude:
             for k in bfrange:
	        for nm in datasets:
                  dirbaseline = results_dir + baseline + "/" + solver + "/"
		
		  try:
                     with open(dirbaseline + solver + "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out",'r') as fl:
                       if 'PASSED' in fl.read():
			       y_true =0
		       else: y_true =1

		     msg=dirbaseline + solver + "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out"
		     print msg

		  except IOError: 
                     msg = "File does not exist =" + dirbaseline + solver + "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out"
		     print msg
		     continue

		  msg =results_dir+detector+"/"+solver+"/"+params[pindex]+"/"+solver+"_"+detectors[index]+"_"+paramstr[pindex]+"_"+str(i)+"_"+str(j)+"\
_"+str(k)+"_"+nm+".out"
		  #print msg
                  try:
                    with open(results_dir+detector+"/"+solver+"/"+params[pindex]+"/"+solver+"_"+detectors[index]+"_"+paramstr[pindex]+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out",'r') as fl:
                       if (detectors_text[index] in fl.read()):
			       y_pred =1
                       else:
                               y_pred =0

		  except IOError:
                    msg = "File does not exist=" + results_dir+detector+"/"+solver+"/"+params[pindex]+"/"+solver+"_"+detectors[index]+"_"+paramstr[pindex]+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out"
		    print msg
		    continue

		  total+=1
		  #obtain TF=TN=FP=FN
		  if y_true==y_pred==1:
                        TP += 1
		  if y_pred==1 and y_true!=y_pred:
                        FP += 1
		  
		  if y_true==y_pred==0:
                        TN += 1

		  if y_pred==0 and y_true!=y_pred:
                        FN += 1

       with open("./data/detectors/"+detector+"_"+solver+"_"+paramstr[pindex]+"_"+bftype+".dat",'w') as fls: 
	       if (TP+TN+FP+FN) == 0:
		       accuracy = 0.0
	       else:
		       accuracy=(TP+TN)/(TP+TN+FP+FN)

	       if (TP+FP) == 0:
		       precision = 0.0
	       else:
		       precision=TP/(TP+FP)

	       if (TP+FN) == 0:
		       recall = 0.0
	       else:
		       recall=TP/(TP+FN)

	       if (precision + recall) == 0:
		       fscore = 0.0
	       else:
		       fscore= 2 *(precision * recall) / (precision + recall)

	       msg=str(TP)+";"+str(TN)+";"+str(FP)+";"+str(FN)+";"+str(accuracy)+";"+str(precision)+";"+str(recall)+";"+str(fscore)
	       fls.write(msg + '\n')
       fls.close()
