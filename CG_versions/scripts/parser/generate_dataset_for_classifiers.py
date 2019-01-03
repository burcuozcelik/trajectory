from __future__ import division
import os
import sys
from sys import argv

cg_home=os.environ["SRC_HOME"]
simulationdir=cg_home + "/results/multibit/baseline/"
ml=cg_home + "/results/multibit/ml/"
extension = "_baseline"

#solvers = ['cg']
distributions=['baseline']
#datasets = ['s3rmt3m1', 's3rmt3m3', 'sts4098']

#datasets= ['af_shell3', 'af_shell4', 'af_shell7', 'af_shell8', 'bcsstk13', 'bcsstk14', 'bcsstk15', 'bcsstk16', 'bcsstk24', 'bcsstk27', 'bcsstk28', 'bcsstk38', 'ex13', 'ex15', 'ex3', 'ex9', 'Kuu', 'msc04515', 'nasa2146', 'Pres_Poisson', 's1rmq4m1', 's1rmt3m1', 's2rmq4m1', 's2rmt3m1', 's3rmq4m1', 's3rmt3m1', 's3rmt3m3', 'sts4098']

if len(argv) < 3:
    print("Usage: solver; dataset")
    sys.exit(1)
else:
    solvers = []
    solvers.append(argv[1])
    print solvers
    
    datasets = []
    datasets.append(argv[2])
    print datasets


def is_null(value):
    if value == "nan":
        return "-1"
    else:
        return value

def parse_data_baseline(path, fname):
    global fiwhen, liter, itr_baseline, res_norm, orth_ex1, orth_ex2, newsum_ex1, newsum_ex2, newsum_scaler

    ifile= path + fname
    values = []
    orth_ex1 = []
    orth_ex2 = []
    newsum_scaler = []
    
    with open(ifile,'r') as inp:
        for line in inp:
            values = str(line).split(';')
            if(len(values)>1 and values[0] == "RESIDUAL_NORM"):
                res_norm = values[1:-1:]
            elif(len(values)>1 and values[0] == "ORTH_EX1"):
                orth_ex1 = values[1:-1:]
            elif(len(values)>1 and values[0] == "ORTH_EX2"):
                orth_ex2 = values[1:-1:]
            elif(len(values)>1 and values[0] == "NEWSUM_EX1"):
                 newsum_ex1 = values[1:-1:]
            elif(len(values)>1 and values[0] == "NEWSUM_EX2"):
                newsum_ex2 = values[1:-1:]
            elif(len(values)>1 and values[0] == "NEWSUM_SCALER"):
                newsum_scaler = values[1:-1:]
            elif "Baseline iterations" in line:
                itr_baseline=line[21:-1:]
            elif "Last iteration found" in line:
                liter = line[22:-1:]


    if len(newsum_scaler) > 0:
        rdata = ""
        for i in range(0, len(res_norm)):
            rdata= rdata + "-1"
            rdata = rdata + " 1:" + is_null(res_norm[i]) + " 2:" + is_null(newsum_ex1[i]) + " 3:" + is_null(newsum_ex1[i]) + " 4:" + is_null(newsum_scaler[i])

            if len(orth_ex1) > 0:
                rdata = rdata + " 5:" + is_null(orth_ex1[i]) + " 6:" + is_null(orth_ex2[i])
            #else:
             #   print "gokcen"
             #   print ifile

            if i != len(res_norm):
                rdata = rdata + "\n"

        return rdata
    else:
        return ""


def parse_data(path, fname):
    global fiwhen, liter, itr_baseline, res_norm, orth_ex1, orth_ex2, newsum_ex1, newsum_ex2, newsum_scaler

    ifile= path + fname
    #print ifile

    values = []
    orth_ex1 = []
    orth_ex2 = []
    newsum_scaler = []
    print ifile
    with open(ifile,'r') as inp:
        for line in inp:
            values = str(line).split(';')
            if(len(values)>1 and values[0] == "RESIDUAL_NORM"):
                res_norm = values[1:-1:]
            elif(len(values)>1 and values[0] == "ORTH_EX1"):
                orth_ex1 = values[1:-1:]
            elif(len(values)>1 and values[0] == "ORTH_EX2"):
                orth_ex2 = values[1:-1:]
            elif(len(values)>1 and values[0] == "NEWSUM_EX1"):
                 newsum_ex1 = values[1:-1:]
            elif(len(values)>1 and values[0] == "NEWSUM_EX2"):
                newsum_ex2 = values[1:-1:]
            elif(len(values)>1 and values[0] == "NEWSUM_SCALER"):
                newsum_scaler = values[1:-1:]
            elif "Baseline iterations" in line:
                itr_baseline=line[21:-1:]
            elif "Place in iterations space" in line:
                fiwhen =  line[34:-1:]
            elif "Last iteration found" in line:
                liter = line[22:-1:]
    inp.close()
    
    if len(newsum_scaler) > 0:
        rdata = ""
        for i in range(0, len(res_norm)):
            if int(liter) != int(itr_baseline) and i >= int(fiwhen):
                 rdata= rdata + "+1"
            else:
                rdata= rdata + "-1"
            rdata = rdata + " 1:" + is_null(res_norm[i]) + " 2:" + is_null(newsum_ex1[i]) + " 3:" + is_null(newsum_ex1[i]) + " 4:" + is_null(newsum_scaler[i])

            if len(orth_ex1) > 0:
                rdata = rdata + " 5:" + is_null(orth_ex1[i]) + " 6:" + is_null(orth_ex2[i])
        
            if i != len(res_norm):
                rdata = rdata + "\n"

        return rdata
    else:
        return ""

    
def main():
    for solver in solvers:
        for dataset in datasets:
            #rpath = simulationdir + solver + "/" + dataset + "/" # for baseline
            rpath = simulationdir 
            for ds in distributions:
                wfname = ml + solver + "_" + dataset + "_" + ds + ".dat" # for baseline
                #print wfname
                fdata = open( wfname, "w") 
                data = ""
                #print "searching files"
                #print rpath
                for path, subdirs, files in os.walk(rpath):
                    for dfile in files:
                        #print "_"+ds
                        
                        #if  "" in dfile:
                        if solver+"_"+dataset+"_baseline.dat" == dfile:
                            print "Parsing ", path+dfile
                            rdata = parse_data_baseline(path, dfile)
                            if len(rdata) > 0:
                                data = data + rdata
                print "Writing for " + solver + "_" + dataset + "_" + ds
                if len(data) > 0:
                    fdata.write(data)
                    #print "writing"
                else:
                    emsg = "ERROR: There is no data for " + solver + "_" + dataset + "_" + ds
                    print emsg
                
                fdata.close()
main()
