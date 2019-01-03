from __future__ import division
import os
import sys
from sys import argv
import random 
from random import shuffle


cg_home=os.environ["SRC_HOME"]
ml=cg_home + "/results/multibit/ml-50/"
ml_training=cg_home + "/results/multibit/ml-50/training/"
ml_testing=cg_home + "/results/multibit/ml-50/testing/"
#baseline=cg_home + "/results/multibit/baseline/"

#solvers=['cg']
#solvers = ['cg', 'cgs', 'bicg', 'bicgsta', 'iccg']

distributions=['normal']
#datasets = ['af_shell3', 'ex3']
datasets= ['af_shell3', 'af_shell4', 'af_shell7', 'af_shell8', 'bcsstk13', 'bcsstk14', 'bcsstk15', 'bcsstk16', 'bcsstk24', 'bcsstk27', 'bcsstk28', 'bcsstk38', 'ex13', 'ex15', 'ex3', 'ex9', 'Kuu', 'msc04515', 'nasa2146', 'Pres_Poisson', 's1rmq4m1', 's1rmt3m1', 's2rmq4m1', 's2rmt3m1', 's3rmq4m1', 's3rmt3m1', 's3rmt3m3', 'sts4098']

max_limit_testing_per_dataset=100000
training_set_ratio = 0.9
negative_set_ratio = 0.5

if len(argv) < 3:
    print("Usage: number of datasets to be used in training; solver")
    sys.exit(1)
else:
    ntraining_dataset= int(argv[1])
    #temporal
    solvers = []
    solvers.append(argv[2])
    print solvers
    #sys.exit(1)
 
    
def main():
    tdataset_index=[]
    seletected_datasets=""
    while 1:
    #for i in range(0, ntraining_dataset):
        rvalue = random.randint(0, len(datasets) -1)
        if not rvalue in tdataset_index:
            seletected_datasets = seletected_datasets + datasets[rvalue] + ";"
            tdataset_index.append(rvalue)
        
        if len(tdataset_index) == ntraining_dataset:
            break

    print "Selected datasets: ", seletected_datasets
    total_training_size=0
    total_testing_size=0

    for solver in solvers:
        for dist in distributions:
            testing_file= ml_testing + solver + "_" + dist + "_numDS" + str(ntraining_dataset) + ".test" 
            training_file= ml_training + solver + "_" + dist + "_numDS" + str(ntraining_dataset) + ".train"
            ftraining = open(training_file, "w")
            ftesting = open(testing_file, "w")
            
            for ti in tdataset_index:
                cdataset=[]
                fname = ml + solver + "_" + datasets[ti] + "_" + dist + ".dat"
                with open(fname,'r') as inp:           
                    for line in inp:
                        cdataset.append(line.split('\n')[0])
                
                inp.close()

                chosen_training_indeces=[]
                training_size = int(len(cdataset) * training_set_ratio)
                all_training_indeces=[1] * len(cdataset)
                
                #selected n unique random number
                num_ts=0
                while num_ts < training_size:
                    rvalue = random.randint(0,len(cdataset)-1) 
                    if all_training_indeces[rvalue] == 1:
                        all_training_indeces[rvalue] = -1
                        chosen_training_indeces.append(rvalue)
                        num_ts = num_ts + 1

                msg ="The number of random ids generated for " + ml + solver + "_" + datasets[ti] + "_" + dist + "is " + str(len(chosen_training_indeces))
                print msg      
                     
                training_negative_samples=[]
                training_positive_samples=[]
                training_total_samples=[]
                chosen_training=[]
                
                testing_negative_samples=[]
                testing_positive_samples=[]
                testing_total_samples=[]

                    
                for i in range(0,len(all_training_indeces)):
                    label=cdataset[i].split(" ")[0]
                    if all_training_indeces[i] == -1: #training
                        training_total_samples.append(cdataset[i])
                        if label == "-1":
                            training_negative_samples.append(cdataset[i])
                        else:
                            training_positive_samples.append(cdataset[i])
                    else:
                        testing_total_samples.append(cdataset[i])
                        if label == "-1":
                            testing_negative_samples.append(cdataset[i])
                        else:
                            testing_positive_samples.append(cdataset[i])

                print "Testing size from training set: ", len(testing_positive_samples) + len(testing_negative_samples) 
                print "Negative native", len(training_negative_samples)
                print "Positive native", len(training_positive_samples)

                negative_samples_size_in_training = len(training_negative_samples)
                positive_samples_size_in_training = int((negative_samples_size_in_training * (1 - negative_set_ratio)) / negative_set_ratio)
                if len(training_positive_samples) < positive_samples_size_in_training:
                    #print "Negative", len(training_negative_samples)
                    #print "Postive", len(training_positive_samples)
                    print "Not enough positive samples"
                    #sys.exit(1)
                    positive_samples_size_in_training = len(training_positive_samples)
                    negative_samples_size_in_training = int((positive_samples_size_in_training * negative_set_ratio) / (1- negative_set_ratio))

                
                print "Negative chosen", negative_samples_size_in_training
                print "Positive chosen", positive_samples_size_in_training 

                shuffle(training_positive_samples)
                for i in range(0, positive_samples_size_in_training):
                    chosen_training.append(training_positive_samples[i])
                
                shuffle(training_negative_samples)
                for i in range(0, negative_samples_size_in_training):
                    chosen_training.append(training_negative_samples[i])
                

                fname = ml + solver + "_" + datasets[ti] + "_baseline" + ".dat"
                with open(fname,'r') as inp:
                    for line in inp:
                        chosen_training.append(line.split('\n')[0])
                        chosen_training.append(line.split('\n')[0])
                        #chosen_training.append(line.split('\n')[0])

                ##################NEW#####################
            
                total_training_size= total_training_size + len(chosen_training)
                shuffle(chosen_training)
                shuffle(chosen_training)
                shuffle(chosen_training)
                for lindex in range(0,len(chosen_training)):       
                    ftraining.write(training_total_samples[lindex] + "\n")   


                testing_size=0
                if len(testing_negative_samples) > len(testing_positive_samples):
                    testing_size=len(testing_positive_samples)
                else:
                    testing_size=len(testing_negative_samples)
                    
                total_testing_size = total_testing_size + (2 * testing_size)
                    
                for lindex in range(0,testing_size):
                    ftesting.write(testing_negative_samples[lindex] + "\n")
                    ftesting.write(testing_positive_samples[lindex] + "\n")

                #testing_size=0
                #if len(testing_negative_samples) > len(testing_positive_samples):
                #    testing_size =  len(testing_positive_samples)
                #else:
                #    testing_size =  len(testing_negative_samples)

                #total_testing_size= total_testing_size + (2 * testing_size)

                #half of the testing is positive samples, the rest consists of negative samples
                #for lindex in range(0,testing_size):
                #    ftesting.write(testing_negative_samples[lindex] + "\n")
                #    ftesting.write(testing_positive_samples[lindex] + "\n")

                ########################################
                        
                #final_training_size=0
                #print "Positive", len(positive_samples)
                #print "Negative", len(negative_samples)
                #if len(positive_samples) >= len(negative_samples):
                 #   final_training_size = len(negative_samples)
                #else:
                 #   final_training_size = len(positive_samples)

                
                #final_testing_size=0
                #if len(testing) >= max_limit_testing_per_dataset:
                #    final_testing_size= max_limit_testing_per_dataset
                #else:
                #    final_testing_size = len(testing)

                #print len(positive_samples)
                #print len(negative_samples)               
                #print len(testing)
                
                #shuffle(positive_samples)
                #shuffle(negative_samples)
                #shuffle(testing)

                #for lindex in range(0,final_training_size):
                 #   ftraining.write(positive_samples[lindex] + "\n")
                  #  ftraining.write(negative_samples[lindex] + "\n")

                                 
                #total_training_size= total_training_size + final_training_size + final_training_size

                #for lindex in range(0,final_testing_size):
                #    ftesting.write(testing[lindex] + "\n")
                 
                #total_testing_size= total_testing_size + final_testing_size
                #print final_training_size
                #print final_testing_size
            
                        
            for i in range(0, len(datasets)):
                if not i in tdataset_index:
                     testing_total_samples=[]
                     testing_negative_samples=[]
                     testing_positive_samples=[]
                     fname = ml + solver + "_" + datasets[i] + "_" + dist + ".dat"
                     with open(fname,'r') as inp:
                         for line in inp:
                             testing_total_samples.append(line.split('\n')[0])
                             label=line.split(" ")[0]
                             if label == "-1":
                                 testing_negative_samples.append(line.split('\n')[0])
                             else:
                                 testing_positive_samples.append(line.split('\n')[0])
                     inp.close()

                     testing_size=0
                     if len(testing_negative_samples) > len(testing_positive_samples):
                         testing_size=len(testing_positive_samples)
                     else:
                         testing_size=len(testing_negative_samples)
                     
                     total_testing_size = total_testing_size + (2 * testing_size)

                     for lindex in range(0,testing_size):
                         ftesting.write(testing_negative_samples[lindex] + "\n")
                         ftesting.write(testing_positive_samples[lindex] + "\n")

            
            ftraining.close()
            ftesting.close()
            
            print "Total training size : " , total_training_size
            print "Negative samples in training", negative_samples_size_in_training
            print "Positive samples in training", positive_samples_size_in_training

            print "Total testing size : " , total_testing_size
            print "Negative samples in testing", total_testing_size/2
            print "Positive samples in testing", total_testing_size/2
            print "Ratio: ", negative_set_ratio
main()
