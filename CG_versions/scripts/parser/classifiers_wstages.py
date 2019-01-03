#!/usr/bin/python

from __future__ import division
from sklearn.ensemble import BaggingClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.ensemble import ExtraTreesClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import AdaBoostClassifier
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.svm import SVC
from sklearn.gaussian_process import GaussianProcessClassifier
from sklearn.gaussian_process.kernels import RBF
from sklearn.naive_bayes import GaussianNB
from sklearn import ensemble
from sklearn import linear_model
from sklearn.svm import SVR
from sklearn import tree
from sklearn.neighbors import KNeighborsRegressor
import numpy as np
from sklearn.model_selection import cross_val_score
from sklearn import model_selection
from sklearn import preprocessing
from sklearn.metrics import accuracy_score
from sklearn.metrics import precision_score
from sklearn.metrics import recall_score
from sklearn.metrics import f1_score
from sklearn.metrics import confusion_matrix
import cPickle
from timingModule import *
import sys, getopt
import os.path
import math

def print_usage():
	print 'Usage:'
	print sys.argv[0], '-s <stage> [-t <trainingfile> -x <testingfile> -m <modelfile>]'
	print
	print 'Example'
	print sys.argv[0], '-s 1 -t <trainingfile>.tran1 -x <testingfile>.test1'	
	print sys.argv[0], '-s 2 -t <trainingfile>.tran2 -a <trainingfile>.tran1.best'	
	print sys.argv[0], '-s 3 -x <testingfile>.test2 -m <modelfile>.mod'
	print sys.argv[0], '-s 4 -t <trainingfile>.tran -x <testingfile>.test -i <model_name>'

def compareML(model, name, X_train, y_train, seed, metric):
        results = []
        names = []
        kfold = model_selection.KFold(n_splits=10, random_state=seed)
	X_train_scaled = preprocessing.normalize(X_train)

        cv_results = model_selection.cross_val_score(model, X_train_scaled, y_train, cv=kfold, scoring=metric)
        results.append(cv_results)
        names.append(name)
        msg = "Comparision-%s %s: %f (%f)" % (metric, name, cv_results.mean(), cv_results.std())
        print(msg)


def obtain_TP_FP_TN_FN(y_true, y_pred):
	TP = 0
	FP = 0
	TN = 0
	FN = 0
	
	for i in range(len(y_pred)): 
		if y_true[i]==y_pred[i]==1:
			TP += 1

	for i in range(len(y_pred)): 
		if y_pred[i]==1 and y_true[i]!=y_pred[i]:
			FP += 1

	for i in range(len(y_pred)): 
		if y_true[i]==y_pred[i]==0:
			TN += 1

	for i in range(len(y_pred)): 
		if y_pred[i]==0 and y_true[i]!=y_pred[i]:
			FN += 1

	return(TP, FP, TN, FN)


def file_load(fname, X, Y):
	#print fname
	with open(fname,'r') as fls:
		for line in fls:
			lst1 = str(line).split()
			#print lst1[0]
			x = float(lst1[0])
			temp = []
			
			for st in lst1[1:len(lst1)]:
				value = float(st.split(":")[1])
				if math.isnan(value) or math.isinf(value):
					value = -1
					print "Error: Input contains NaN or infinity: ", st
					#sys.exit(1)
				temp.append(value)
			X.append(temp)
				
				
			if(x==-1):
				Y.append(0)
			else:  
				Y.append(1)
			
	fls.close()


def get_stats(y_true, y_pred):
        fscore = 0

	TP,FP,TN,FN = obtain_TP_FP_TN_FN(y_true, y_pred)

        #confusion_matrix does not work for regression                                                                                                                     
	CM = confusion_matrix(y_true, y_pred)                                                                                                                             
	TN1 = CM[0][0]                                                                                                                                                     
	FN1 = CM[1][0]                                                                                                                                                    
	TP1 = CM[1][1]                                                                                                                                                     
	FP1 = CM[0][1]                                                                                                                                                     
	
	w, h = 2, 2
	classes = [[0 for x in range(w)] for y in range(h)]
	
	classes [0][0] = 0
	classes [0][1] = 0
	classes [1][0] = 0
	classes [1][1] = 0

	for i in range(len(y_true)):
		if y_true[i] == 0:
			if y_true[i] == y_pred[i]:
				classes[0][0] = classes[0][0] + 1
			else:
				classes[0][1] = classes[0][1] + 1
		else:
			if y_true[i] == y_pred[i]:
				classes[1][0] = classes[1][0] + 1
			else:
				classes[1][1] = classes[1][1] + 1

        #Just to make sure                                                                                                                                                
	if (TP + TN + FP + FN) == 0:
		gaccuracy = 0.0
	else:
		gaccuracy = ((TP + TN) / (TP + TN + FP + FN)) * 100
		
	if (TP + FP) == 0:
		gprecision = 0.0
	else:
		gprecision = (TP / (TP + FP)) * 100
		
	if (TP + FN) == 0:
		grecall = 0.0
	else:
		grecall = (TP / (TP + FN)) * 100
		
	if (gprecision + grecall) == 0:
		gfscore = 0
	else:
		gfscore= 2 * ((gprecision * grecall) / ( gprecision + grecall))
	
	if (classes[0][0] + classes[0][1]) == 0:
		acc_class0 = 0.0
	else:
		acc_class0= classes[0][0] * 100 / (classes[0][0] + classes[0][1])
		
	if (classes[1][0] + classes[1][1]) == 0:
		acc_class1 = 0.0
	else:
		acc_class1= classes[1][0] * 100 / (classes[1][0] + classes[1][1])

	if gfscore > fscore:
		fscore = gfscore
	
        return {'gfscore':gfscore, 'gaccuracy':gaccuracy,'gprecision':gprecision,'grecall':grecall, 'TP':TP,'TN':TN,'FP':FP,'FN':FN, 'acc_class0':acc_class0, 'acc_class1':acc_class1, 'fscore':fscore}


def run_stage1(trainingfile, testingfile):
	X_train = []
	y_train = []
	X_test = []
	y_true = []

	best_cid = -1
	best_gfscore = 0

	print 'Running stage 1 on training ', trainingfile, ' and testing ', testingfile
	file_load(trainingfile, X_train, y_train)
	file_load(testingfile, X_test, y_true)
	

	fname_all=trainingfile+".all"
	fname_best=trainingfile+".best"

	X_train_scaled = preprocessing.normalize(X_train)
	X_test_scaled = preprocessing.normalize(X_test)

	with open(fname_all,'w') as fall:
		for cid, name, model in models:
			#print name
			#fix here later
			#fname_model=trainingfile+'.model'+cid
			fname_model=trainingfile+'.model'
			
			setStartTime()
			model.fit(X_train_scaled, y_train)
			setEndTime()
			training_time=getElapsedTime()

			with open(fname_model,'w') as fmodel:
				cPickle.dump(model, fmodel)   

			setStartTime() 
			y_pred = model.predict(X_test_scaled)
			setEndTime()
			testing_time=getElapsedTime()
    
			rdata = get_stats(y_true, y_pred)
			#msg= "%s;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%f;%f;%d;%d" % (cid, rdata['gfscore'], rdata['gaccuracy'],rdata['gprecision'],rdata['grecall'], precision_score(y_true, y_pred, average='binary') * 100, recall_score(y_true, y_pred, average='binary')*100, rdata['TP'],rdata['TN'],rdata['FP'],rdata['FN'], rdata['acc_class0'], rdata['acc_class1'],training_time,testing_time)
			#print msg
			
			if float(rdata['gfscore']) > best_gfscore:
				best_gfscore = float(rdata['gfscore'])
				best_cid = cid

			msg = "Precision: " + str(precision_score(y_true, y_pred, average='binary') * 100) + " Recall: " + str(recall_score(y_true, y_pred, average='binary')*100) + " Fscore: " + str(f1_score(y_true, y_pred, average='binary') * 100) + " Accurancy: " + str(accuracy_score(y_true, y_pred) * 100)
			print msg
			fall.write(msg + '\n')

		with open(fname_best,'w') as fbest:
			msg="%s;%f" % (best_cid,best_gfscore)
			fbest.write(msg + '\n')
			print 'Best algorithm: ', models[int(best_cid)][1], ' GF Score: ', best_gfscore
		

def run_stage2(trainingfile, algfile):
	X_train = []
        y_train = []
        
        print 'Running stage 2 on training ', trainingfile, ' and algorithm ', algfile
        file_load(trainingfile, X_train, y_train)
	X_train_scaled = preprocessing.normalize(X_train)
        
	try:
		with open(algfile,'r') as falg:
			for line in falg:
				bestalg=int(line.split(";")[0])
	except IOError:
		print 'Algorithm files does not exist'
		raise SystemExit


	#print bestalg
	fname_model=trainingfile+".model"
	fname_time=trainingfile+".time"

	#bestalg index == bestalg - 1
	ibestalg = bestalg - 1
	model = models[ibestalg][2]
	print models[ibestalg][0]
	print models[ibestalg][1]
	setStartTime()

	
	model.fit(X_train_scaled, y_train)
	setEndTime()
	training_time=getElapsedTime()
		
	with open(fname_model,'w') as fmodel:
		cPickle.dump(model, fmodel)   

	with open(fname_time,'w') as ftime:
		msg="%d" % (training_time)
		ftime.write(msg + '\n')

def run_stage3(modelfile,testingfile):
	X_test = []
	y_true = []

	file_load(testingfile, X_test, y_true)
	X_test_scaled = preprocessing.normalize(X_test)


	stest=modelfile.split("_")
	chunksize=stest[1]
	print chunksize
	with open(modelfile, 'rb') as fmodel:
		model = cPickle.load(fmodel)



	printed=0
	predfile=testingfile+'.pred'
	for i in range(0,len(X_test_scaled)):
		y_pred = model.predict(X_test_scaled[i:i+1])
		if y_pred == 1:
			print "Detected " + `i`

			with open(predfile, 'w') as pfile:
				iternum= int(chunksize) * (i + 1)
				print iternum
				pfile.write('[ML Check] Bit error detected, terminating application\n')
				pfile.write('Failed at iteration: '+ str(iternum) +'\n')
				pfile.write('Last iteration found: '+ str(iternum) +'\n')

			#pfile.close()
			printed=1
			break

	
	if printed == 0:
		with open(predfile, 'w') as pfile:
			pfile.write('Successfully Completed\n')
		#pfile.close()


def run_stage4(trainingfile, testingfile, modelname):
        X_train = []
        y_train = []
        X_test = []
        y_true = []
        best_cid = -1
        best_gfscore = 0

        print 'Running stage 4 on training ', trainingfile, ' and testing ', testingfile, ' and model name ', modelname
        file_load(trainingfile, X_train, y_train)
        file_load(testingfile, X_test, y_true)

        fmodel=trainingfile+"_"+modelname+".model"
	fstat=trainingfile+"_"+modelname+".stat"
        
	X_train_scaled = preprocessing.normalize(X_train)
        X_test_scaled = preprocessing.normalize(X_test)

	is_model_exist=False
	for cid, name, model in models:
		if modelname == name:
			is_model_exist = True
			break

	if not is_model_exist:
		print "Error: Model does not exist: ", modelname 
		sys.exit()

	setStartTime()
	model.fit(X_train_scaled, y_train)
	setEndTime()
	training_time=getElapsedTime()
	 
	with open(fmodel,'w') as fp:
		cPickle.dump(model, fp)
	fp.close()

	setStartTime()
	y_pred = model.predict(X_test_scaled)
	setEndTime()
	testing_time=getElapsedTime()

	msg = "Precision: " + str(precision_score(y_true, y_pred, average='binary') * 100) + " Recall: " + str(recall_score(y_true, y_pred, average='binary')*100) + " Fscore: " + str(f1_score(y_true, y_pred, average='binary') * 100) + " Accurancy: " + str(accuracy_score(y_true, y_pred) * 100) + " TrainingTime: " + str(training_time) + " TestingTime: " + str(testing_time)
	
	with open(fstat,'w') as fp:
		fp.write(msg + '\n')
	fp.close()


def main(argv):
	testingfile  = ''
	trainingfile = ''
	modelfile    = ''
	stage        = 0
	algfile      = ''
	modelName    = ''

	global models
	models = []
	ntrees=1000

	print argv
	try:
		opts, args = getopt.getopt(argv,"hs:t:x:m:a:i:",["stage=","training=","testing=","model=","algorithm=","modelName="])
	except getopt.GetoptError:
		print_usage()
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print_usage()
			sys.exit()
		elif opt in ("-s", "--stage"):
			stage = int(arg)
		elif opt in ("-t", "--training"):
			trainingfile = arg
		elif opt in ("-x", "--testing"):
			testingfile = arg
		elif opt in ("-m", "--model"):
			modelfile = arg
		elif opt in ("-a", "--algorithm"):
			algfile = arg
		elif opt in ("-i", "--modelName"):
			modelName = arg

		
	print 'Stage          = ', stage
	print 'Trainingfile   = ', trainingfile
	print 'Testing file   = ', testingfile
	print 'Model file     = ', modelfile
	print 'Algorithm file = ', algfile
	print 'Model Name =',  modelName
	
	if stage == 0:
		print 'Error: stage parameter required'
		print_usage()
		sys.exit()

	if stage == 1 and (os.path.isfile(trainingfile) == False or os.path.isfile(testingfile) == False):
		print "Error: missing/erroneous parameters"
		print_usage()
		sys.exit()
	
	if stage == 2 and (os.path.isfile(testingfile) == False or os.path.isfile(algfile) == False):
                print "Error: missing/erroneous parameters"
                print_usage()
                sys.exit()

	if stage == 3 and (os.path.isfile(testingfile) == False or os.path.isfile(modelfile) == False):
		print "Error: missing/erroneous parameters"
		print_usage()
		sys.exit()

	if stage == 4 and (os.path.isfile(trainingfile) == False or os.path.isfile(testingfile) == False or modelName == ''):
		print "Error: missing/erroneous parameters"
                print_usage()
                sys.exit()

	
	models.append(('1','Bagging', BaggingClassifier(KNeighborsClassifier(), max_samples=0.5, max_features=0.5))) 
	models.append(('2', 'DecisionTree', DecisionTreeClassifier(max_depth=None, min_samples_split=2, random_state=0)))
	models.append(('3', 'RandomForest', RandomForestClassifier(n_estimators=10, max_depth=None, min_samples_split=2)))
	models.append(('4', 'ExtraTrees', ExtraTreesClassifier(n_estimators=10, max_depth=None, min_samples_split=2)))
	models.append(('5', 'AdaBoost', AdaBoostClassifier(n_estimators=100)))
	models.append(('6', 'GradientBoosting', GradientBoostingClassifier(n_estimators=100, learning_rate=1.0, max_depth=1)))
	models.append(('7','GaussianNB', GaussianNB()))
	models.append(('8','SVM', SVC(gamma=2, C=1)))
	models.append(('9','MLPC', MLPClassifier(alpha=1)))
	models.append(('10','GaussianProcess', GaussianProcessClassifier(1.0 * RBF(1.0), warm_start=True)))

	#models.append(('11','LinearReg', linear_model.LinearRegression()))
	#models.append(('12','KNeighborsReg', KNeighborsRegressor(n_neighbors=2)))
	#models.append(('13','DecisionTreeReg', tree.DecisionTreeRegressor()))
	#models.append(('14','AdaBoostReg', ensemble.AdaBoostRegressor(n_estimators=ntrees)))
	#models.append(('15','SVR', SVR(kernel='rbf', C=1e3, gamma=0.1)))	
        #clf10 = linear_model.LinearRegression()
        #clf11 = KNeighborsRegressor(n_neighbors=2)
        #clf12 = tree.DecisionTreeRegressor()
        #clf13 = ensemble.AdaBoostRegressor(n_estimators=ntrees)
        #clf14 = SVR(kernel='rbf', C=1e3, gamma=0.1)

	if stage == 1:
		run_stage1(trainingfile, testingfile)
	elif stage == 2:
		run_stage2(trainingfile, algfile)
	elif stage == 3:
		run_stage3(modelfile, testingfile)
	elif stage == 4:
                run_stage4(trainingfile, testingfile, modelName)
	else:
		print 'Error: Stage not supported'
		sys.exit()

	print 'Done.'

if __name__ == "__main__":
	main(sys.argv[1:])
