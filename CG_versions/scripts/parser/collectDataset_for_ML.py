from __future__ import division
import sys
from sys import argv
from os import walk

solvers = ["cg"] 


#fault injection
bfrange_all = range(0,64)
bfrange_sign= range(63,64)
bfrange_exponent=range(52,63)
bfrange_mantissa=range(0,52)

dataset_size=[28]
#fi_size = [100]
chunk_size = [10]
#iterations= [1]


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

detector="ml" 
baseline = "baseline"

for solver in solvers:
	TP=TN=FP=FN=total=0
	mlfiles = []
	dirbaseline = results_dir + baseline + "/" + solver + "/"
	for ch in chunk_size:
	     for dt in dataset_size:
		     for fi in fi_size:
			     for index in iterations:
				     mldir=results_dir + detector + "/" + solver +  "/" + str(ch) + "_" + str(dt) + "_" + str(fi) + "_" + str(index) + "/"
				     print mldir
				     
				     for (dirpath, dirnames, filenames) in walk(mldir):
					     mlfiles.extend(filenames)
					     break
					     
				     #print mlfiles
				     #cg_10_4_100_1_woDetectors_Kuu_3_0.8_25.test2.pred
				     #cg_baseline_2_0.6_0_af_shell7.out
				     for mlfile in mlfiles:
					     ml_split = mlfile.split("_")
					     if ml_split[6] == "af" or ml_split[6] == "Pres":
						     mag=ml_split[10].split(".")
						     baseling_fname = ml_split[0] + "_baseline_" + ml_split[8] + "_" + ml_split[9] + "_" + mag[0] + "_" + ml_split[6] + "_" + ml_split[7] + ".out"
					     else:
						     mag=ml_split[9].split(".")
						     baseling_fname = ml_split[0] + "_baseline_" + ml_split[7] + "_" + ml_split[8] + "_" + mag[0] + "_" + ml_split[6] + ".out"
					     try:
						     with open(dirbaseline + "/" + baseling_fname ,'r') as fl:
							     if 'PASSED' in fl.read():
								     y_true =0
							     else: y_true =1
					     except IOError: 
						     msg = "File does not exist =" + dirbaseline + "/" + baseling_fname
						     print msg
						     continue

					     try:
  
						     with open(mldir + mlfile,'r') as fl:
							     if ("detected" in fl.read()):
								     y_pred =1
							     else:
								     y_pred =0
								     
					     except IOError:
						     msg = "File does not exist=" + mlfile
						     print msg
						     continue

					     total+=1

					     if y_true==y_pred==1:
						     TP += 1

					     if y_pred==1 and y_true!=y_pred:
						     FP += 1
						     
					     if y_true==y_pred==0:
						     TN += 1
								     
					     if y_pred==0 and y_true!=y_pred:
						     FN += 1

				     with open("gokcen",'w') as fls: 
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
