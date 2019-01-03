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
from sklearn.metrics import confusion_matrix
import sys
from sys import argv
from timingModule import *

if len(argv) < 3:
        print("Usage: TrainingFile TestingFile", (argv[0]))
        sys.exit(1)

def compareML(model, name, X_train, y_train, seed, metric):
        results = []
        names = []
        kfold = model_selection.KFold(n_splits=10, random_state=seed)
        cv_results = model_selection.cross_val_score(model, X_train, y_train, cv=kfold, scoring=metric)
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

def main():
	models = []
	ntrees=1000
	
	models.append(('1','Bagging', BaggingClassifier(KNeighborsClassifier(), max_samples=0.5, max_features=0.5))) 
	models.append(('2', 'DecisionTree', DecisionTreeClassifier(max_depth=None, min_samples_split=2, random_state=0)))
	models.append(('3', 'RandomForest', RandomForestClassifier(n_estimators=10, max_depth=None, min_samples_split=2)))
	models.append(('4', 'ExtraTrees', ExtraTreesClassifier(n_estimators=10, max_depth=None, min_samples_split=2)))
	models.append(('5', 'AdaBoost', AdaBoostClassifier(n_estimators=100)))
	models.append(('6', 'GradientBoosting', GradientBoostingClassifier(n_estimators=100, learning_rate=1.0, max_depth=1)))
	models.append(('7','GaussianNB', GaussianNB()))
	models.append(('8','SVM', SVC(gamma=2, C=1)))
	models.append(('9','GaussianProcess', GaussianProcessClassifier(1.0 * RBF(1.0), warm_start=True)))
	models.append(('10','MLPC', MLPClassifier(alpha=1)))
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

	X_train = []
	y_train = []
	X_test = []
	y_true = []

	with open(argv[1],'r') as fls:
		for line in fls:
			lst1 = str(line).split()
			x = float(lst1[0])
			temp = []

			for st in lst1[1:len(lst1)]:
				temp.append(float(st.split(":")[1]))
			X_train.append(temp)

		        
			if(x==-1):
				y_train.append(0)
			else:  y_train.append(1)
			
	fls.close()

	with open(argv[2],'r') as fls:
		for line in fls:
			lst1 = str(line).split()
			x = float(lst1[0])
			temp = []

			for st in lst1[1:len(lst1)]:
				temp.append(float(st.split(":")[1]))
			X_test.append(temp)
			if(x==-1):
				y_true.append((0)) 
			else:  y_true.append(1) 
		
	fls.close()


	#X_train_scaled=preprocessing.scale(X_train)
        #X_test_scaled=preprocessing.scale(X_test)
	#scaling does not look helping

	X_train_scaled = X_train
	X_test_scaled = X_test

	for cid, name, model in models:
		setStartTime()
		model.fit(X_train_scaled, y_train)
		setEndTime()
		training_time=getElapsedTime()

		setStartTime() 
		y_pred = model.predict(X_test_scaled)
		setEndTime()
                testing_time=getElapsedTime()
    
		TP,FP,TN,FN = obtain_TP_FP_TN_FN(y_true, y_pred)
		
		#confusion_matrix does not work for regression
                #CM = confusion_matrix(y_true, y_pred)
		#TN1 = CM[0][0]
		#FN1 = CM[1][0]
		#TP1 = CM[1][1]
		#FP1 = CM[0][1]

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

		#msg = "%d;%d;%d;%d" %(TP,TN,FP,FN)
		#print msg

		#Just to make sure
		if (TP + TN + FP + FN) == 0:
			gaccuracy = 0.0
		else:
			gaccuracy = ((TP + TN) / (TP + TN + FP + FN)) * 100

		if (TP + FP) == 0:
			gprecision = 0.0
			#print "gprecision 0.0"
		else:
			gprecision = (TP / (TP + FP)) * 100

		if (TP + FN) == 0:
			grecall = 0.0
			#print "grecall 0.0"
		else:
			grecall = (TP / (TP + FN)) * 100

		if (gprecision + grecall) == 0:
			 gfscore = 0
		else:
			gfscore= 2 * ((gprecision * grecall) / ( gprecision + grecall))

		#print gprecision
		#print grecall
		
		if (classes[0][0] + classes[0][1]) == 0:
			acc_class0 = 0.0
		else:
			acc_class0= classes[0][0] * 100 / (classes[0][0] + classes[0][1])

		if (classes[1][0] + classes[1][1]) == 0:
			acc_class1 = 0.0
		else:
			acc_class1= classes[1][0] * 100 / (classes[1][0] + classes[1][1])

  		#if name == "LinearReg" or name == "KNeighborsReg" or name == "DecisionTreeReg" or name == "AdaBoostReg" or name == "SVR":
			#"Model;Accuracy;Precision;Recall;TP;TN;FP;FN;Class;Correct;Wrong;Class;Correct;Wrong;AccIncorrect;AccCorrect;"
		#	msg = "%s;%f;%f;%f;%d;%d;%d;%d;0;%d;%d;1;%d;%d;%f;%f;%d;%d" % (cid,gaccuracy,gprecision,grecall,TP,TN,FP,FN,classes[0][0], classes[0][1], classes[1][0], classes[1][1],acc_class0, acc_class1, training_time, testing_time)
			#msg = "%s;%f;%f;%f;%f;%f;%f" %
		#	print(msg)
		#else:    

		#Model;Fscore;Global;Precision;Recall;Precision;Recall;TP;TN;FP;FN;Correct;Wrong;Correct;Wrong;AccIncorrect;AccCorrect;TrainingTime;TestingTime
		msg = "%s;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;0;%d;%d;1;%d;%d;%f;%f;%d;%d" % (cid, gfscore, gaccuracy,gprecision,grecall, precision_score(y_true, y_pred, average='binary') * 100, recall_score(y_true, y_pred, average='binary')*100, TP,TN,FP,FN,classes[0][0], classes[0][1], classes[1][0], classes[1][1], acc_class0, acc_class1,training_time,testing_time)
		print(msg)

main()    
