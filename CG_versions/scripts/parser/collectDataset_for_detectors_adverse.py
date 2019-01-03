from __future__ import division
import sys
from sys import argv

aid = "aid"
baseline = "baseline"
ssd1 = "ssd1"
newsum = "newsum"
gagan = "gagan"
orth = "orth"

solvers = ["cg", "cgs", "iccg", "bicg", "bicgsta", "qmr"] 
#solvers = ["cg"]
aidparams = ["0.00078125", "0.01","0.001","0.0001","0.00001","0.000001"]
#aidparams = ["0.00078125"]
ssdparams = ["0.00078125","0.01","0.001","0.0001","0.00001","0.000001"]
gaganparams = ["0.1"]
newsumparams = ["1e-08","1e-06","1e-07","1e-08","1e-09","1e-010"]
newsumparamStr = ["0.00000001","0.000001","0.0000001","0.00000001","0.000000001","0.0000000001"]
orthparams = ["1e-08","1e-06","1e-07","1e-08","1e-09","1e-010"]
orthparamStr = ["0.00000001","0.000001","0.0000001","0.00000001","0.000000001","0.0000000001"]

detectors= ["aid","gagan", "newsum", "orth", "ssd1"]
#detectors= ["aid"]
detectors_text = ["Bit", "detected", "detected", "detected", "detected"]


#fault injection
bfrange_all = range(0,64)
bfrange_sign= range(63,64)
bfrange_exponent=range(52,63)
bfrange_mantissa=range(0,52)

inj = [1,2,3]
magnitude = [0.2, 0.4, 0.6, 0.8]

datasets = ['af_shell3', 'af_shell4', 'af_shell7', 'af_shell8', 'bcsstk13','bcsstk14', 'bcsstk15', 'bcsstk16', 'bcsstk24', 'bcsstk27', 'bcsstk28', 'ex3', 'ex9', 'ex13', 'ex15', 'Kuu', 'msc04515', 'bcsstk38', 'nasa2146', 'Pres_Poisson', 'sts4098', 's1rmq4m1', 's1rmt3m1', 's2rmq4m1', 's2rmt3m1', 's3rmq4m1', 's3rmt3m1','s3rmt3m3']

#datasets = ['af_shell3']

def compute_metrics(fpath, tp, fp, tn, fn):
	with open(fpath,'w') as fls: 
		accuracy  = 0.0
		precision = 0.0
		recall    = 0.0
		fscore    = 0.0

		if (tp + tn + fp + fn)  != 0:
			accuracy = (tp + tn)/(tp + tn + fp + fn)

		if (tp + fp) != 0:
			precision = tp/(tp + fp)
				
		if (tp + fn) != 0:
			recall=tp/(tp + fn)
		       
		if (precision + recall) != 0:
			fscore = 2 *(precision * recall) / (precision + recall)

		msg=str(tp)+";"+str(tn)+";"+str(fp)+";"+str(fn)+";"+str(accuracy)+";"+str(precision)+";"+str(recall)+";"+str(fscore)
		print fpath + ' ' + msg
		fls.write(msg + '\n')
	fls.close()



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

     TP_AN = TN_AN = FP_AN = FN_AN = FP_EXTRA_AN = 0
     TP_AD = TN_AD = FP_AD = FN_AD = FP_EXTRA_AD = 0

     for pindex in range(0,len(params)): 
       TP_AN = TN_AN = FP_AN = FN_AN = FP_EXTRA_AN = 0
       TP_AD = TN_AD = FP_AD = FN_AD = FP_EXTRA_AD = 0
       total=0

       for i in inj:
          for j in magnitude:
             for k in bfrange:
	        for nm in datasets:
                  dirbaseline = results_dir + baseline + "/" + solver + "/"
		
		  try:
                     with open(dirbaseline + solver + "_" + baseline + "_" + str(i) + "_" + str(j) + "_" + str(k) + "_" + nm+ ".out",'r') as fl:
			     bl_iter = lf_iter = in_iter = 0
			     lines = fl.readlines()
			     for l in lines:
				     if l.find("Baseline iterations") != -1:
					     bl_iter = int(l.split(':')[1])
				     elif l.find("Last iteration found") != -1:
					     lf_iter = int(l.split(':')[1])
				     elif l.find("Inserted in iteration") != -1:
					     in_iter = int(l.split(':')[1])

		     y_true_anomaly = 0
		     y_true_adverse = 0

		     if lf_iter != bl_iter:
			     y_true_anomaly = 1

		     if lf_iter > 2*bl_iter:
			     y_true_adverse = 1


#                       if 'PASSED' in fl.read():
#		     y_true =0
#		       else: y_true =1

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
			    lines = fl.readlines()
			    y_pred  = 0
			    dt_iter = 0 
			    for l in lines:
				    if l.find(detectors_text[index]) != -1:
					    y_pred = 1
				    elif l.find("Failed at iteration") != -1:
					    dt_iter = int(l.split(':')[1])
		
			    
#		    if (detectors_text[index] in fl.read()):
#						    y_pred =1
#					    else:
#						    y_pred =0

		    if y_pred == 1 and dt_iter < in_iter:
			    if y_true_anomaly:
				    FP_EXTRA_AN += 1
			    if y_true_adverse == 1:
				    FP_EXTRA_AD += 1

		    msg=results_dir+detector+"/"+solver+"/"+params[pindex]+"/"+solver+"_"+detectors[index]+"_"+paramstr[pindex]+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+\
nm+".out"
		    print msg


		  except IOError:
                    msg = "File does not exist=" + results_dir+detector+"/"+solver+"/"+params[pindex]+"/"+solver+"_"+detectors[index]+"_"+paramstr[pindex]+"_"+str(i)+"_"+str(j)+"_"+str(k)+"_"+nm+".out"
		    print msg
		    continue

		  print str(bl_iter) + ' ' + str(lf_iter) + ' ' + str(y_pred) + ' ' + str(dt_iter)

		  total+=1

		  if y_true_anomaly==y_pred==1:
                        TP_AN += 1
		  
		  if y_pred==1 and y_true_anomaly!=y_pred:
                        FP_AN += 1
		  
		  if y_true_anomaly==y_pred==0:
                        TN_AN += 1

		  if y_pred==0 and y_true_anomaly!=y_pred:
                        FN_AN += 1


		  if y_true_adverse==y_pred==1:
                        TP_AD += 1
		  if y_pred==1 and y_true_adverse!=y_pred:
                        FP_AD += 1
		  
		  if y_true_adverse==y_pred==0:
                        TN_AD += 1

		  if y_pred==0 and y_true_adverse!=y_pred:
                        FN_AD += 1
		  
       fpath = "./data/detectors/"+detector+"_"+solver+"_"+paramstr[pindex]+"_"+bftype+"_an.dat"
       compute_metrics(fpath, TP_AN, FP_AN, TN_AN, FN_AN) 

       fpath = "./data/detectors/"+detector+"_"+solver+"_"+paramstr[pindex]+"_"+bftype+"_ad.dat"
       compute_metrics(fpath, TP_AD, FP_AD, TN_AD, FN_AD) 

       fpath = "./data/detectors/"+detector+"_"+solver+"_"+paramstr[pindex]+"_"+bftype+"_an_extra.dat"
       compute_metrics(fpath, TP_AN - FP_EXTRA_AN, FP_AN + FP_EXTRA_AN, TN_AN, FN_AN) 

       fpath = "./data/detectors/"+detector+"_"+solver+"_"+paramstr[pindex]+"_"+bftype+"_ad_extra.dat"
       compute_metrics(fpath, TP_AD - FP_EXTRA_AD, FP_AD + FP_EXTRA_AD, TN_AD, FN_AD) 
