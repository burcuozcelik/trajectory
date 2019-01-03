import os
import sys
import json
import pickle
import pandas as pd


dirname = "/Users/mutl832/Desktop/CG_Versions/CG_versions/Convergence/"
models_home= "/Users/mutl832/Desktop/CG_Versions/CG_versions/results/mlpredict/models/"
exp_list = {}
interval = 5
dsmap = {}


class ExpParams:
    __slots__ = ["inj_itr", "base_itr", "tot_itr","final_itr", "inj_vec", "passed", "err_margin","activation","ref_name","margins","conviters"]
    def __init__(self, inj_itr = -1, base_itr = -1, tot_itr = -1, final_itr=-1, inj_vec = "", passed = "0", err_margin=0.0, activation = -1,ref_name=""):
        self.inj_itr = inj_itr
        self.base_itr = base_itr
        self.tot_itr = tot_itr
        self.final_itr = final_itr
        self.inj_vec=inj_vec
        self.passed = passed
        self.err_margin = err_margin
        self.activation = activation
        self.ref_name = ref_name
        self.margins = {}
        self.conviters = {}

    def get_json(self,key):
        ret_text = ('{' + '"file_name":"' + key + '",'
                    + '"inj_itr":' + str(self.inj_itr) + ','
                    + '"base_itr":' + str(self.base_itr) + ','
                    + '"tot_itr":' + str(self.tot_itr) + ','
                    + '"final_itr":' + str(self.final_itr) + ','
                    + '"inj_vec":"' + self.inj_vec + '",'
                    + '"masked":' + self.passed + ','
                    + '"err_margin":' + str(self.err_margin) + ','
                    + '"activation":' + str(self.activation) + ','
                    + '"ref_name":"' + self.ref_name + '",')
        return ret_text
    def get_combined_json(self):
        if self.ref_name == "":
            return ""
        else:
            ret_text ='{' + '"ref_name":"' + self.ref_name + '"'

            for key,value in self.margins.items():
                ret_text += ',"' + key + '":' + str(value)

            for key,value in self.conviters.items():
                ret_text += ',"' + key + '":' + str(value)

            ret_text += '}'
            return ret_text

    def get_longer_json(self,key):
        ret_text = self.get_combined_json()
        ret_text = ret_text[:-1:]
        ret_text += (',' + '"inj_itr":' + str(self.inj_itr)
                     + ',' + '"base_itr":' + str(self.base_itr)
                     + ',' + '"final_itr":' + str(self.final_itr)
                     + ',' + '"inj_vec":"' + self.inj_vec + '"'
                     + ',' + '"masked":' + self.passed
                     + ',' + '"err_margin":' + str(self.err_margin)
                     + ',' + '"activation":' + str(self.activation)
                     + ',' + '"file_name":"' + key + '"'
                     + '}')
        return ret_text

#end class ExpParams                                                                                                                                               

def isclose(a, b, rel_tol=1e-30, abs_tol=0.0):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)
#end isclose                                                                                                                                           

def calculate_marginal_difference(avec,bvec):
    diff = 0.0
    for i in range(len(avec)):
        if isclose(float(avec[i]),float(bvec[i])):
            continue
        elif isclose(float(avec[i]),0.0):
            if isclose(float(bvec[i]),0.0):
                continue
            else:
                diff += abs((float(bvec[i])-float(avec[i]))/float(bvec[i]))
        else:
            diff += abs((float(avec[i])-float(bvec[i]))/float(avec[i]))
    return diff
#end calculate_marginal_difference 

def check_vector(i, injvec, baselist, comp_type, iters):
    #injvec is the injected points vector, basevec is the normal execution                                                                                                                                
    #i is current injection itr                                                                                                                                             
    #(p)oint of (r)ange comparison                                                                                                                                                     
    # return err_margin and iteration of the comparison                                                                                                               
    if comp_type == 'p':
        min_err = calculate_marginal_difference(injvec,baselist[i])
        return min_err,0.0
    else: #comp_type='r'                     
        beg_itr = 0
        if i >= 20:
            beg_itr = i-20
        end_itr = i + 20
        if end_itr >= iters:
            end_itr = iters-1
        min_err = calculate_marginal_difference(injvec,baselist[beg_itr])
        min_point = beg_itr
        for j in range(beg_itr+1, end_itr):
            basevec = baselist[j]
            diffval = calculate_marginal_difference(injvec,basevec)
            if diffval <= min_err:
                min_err = diffval
                min_point = j
    return min_err,min_point
#end check_vector                                                             


def add_vecline_to_list(line,veclist):
    #first two elements are labels, last element is emptyline                                                                         
    linevec = line.split(';')
    del linevec[0]
    itr = linevec.pop(0)
    lenval =  len(linevec)
    linevec.pop(lenval-1)
    veclist.insert(int(itr)-1,linevec)
   # len(veclist)
#end  add_vecline_to_list 


def GetVecData(vec,fname,shortname):
    datalist = []
    linename = vec + "_vector"
    pars = ExpParams()
    with open(fname) as datafile:
        line = datafile.readline()
        while line:
            if line.startswith(linename):
                add_vecline_to_list(line,datalist)
            line = datafile.readline()
    return datalist
#end Get_Vec_Data

def get_dict(exp_data):
    data = {}
    data['itr_per'] = exp_data.inj_itr*1.0/exp_data.base_itr
    data['x5m'] = exp_data.margins['x5m']
    data['r5m'] = exp_data.margins['r5m']
    data['p5m'] = exp_data.margins['p5m']
    data['x10m'] = exp_data.margins['x10m']
    data['r10m'] = exp_data.margins['r10m']
    data['p10m'] = exp_data.margins['p10m']
    data['x20m'] = exp_data.margins['x20m']
    data['r20m'] = exp_data.margins['r20m']
    data['p20m'] = exp_data.margins['p20m']
    data['vector'] = exp_data.inj_vec
#    print data['itr_per']
#    pd.Series(data).to_frame()

    return data
#end get_dict


def predict(fname,pkl_name):
    vecs = ["r","p","x"]
    res_text = ""
    #find solver and dataset from name
    folders = fname[:fname.rfind('/')]
    p = folders.rfind('/')
    dsname = folders[p+1:]
    p2 = folders[:p].rfind('/')
    sol = folders[p2+1:p]
    ##
    basename = dirname + "baseline/" + sol + "/" + dsname + "/" + sol + "_" + dsname + "_baseline.dat"
    
    exp_data = ExpParams()

    #get injection parameters (1st line:injected vector, 2nd line:injected iteration, 3rd line: baseline iteration, 4th line:error margin)
    with open(fname) as injfile:
        line = injfile.readline().rstrip()
        exp_data.inj_vec = int(line)
        line = injfile.readline().rstrip()
        exp_data.inj_itr = int(line)
        line = injfile.readline().rstrip()
        exp_data.base_itr = int(line)
        line = injfile.readline().rstrip()
        exp_data.err_margin = float(line)
        
    #get error margins for each vector separately for memory consumption                                                                   
    for vec in vecs:
        res_text = ""
        #get baseline data
        baselist = GetVecData(vec,basename,"baseline")
        injlist = GetVecData(vec,fname,"inj")
        ##compare injlist and baselist for 10th 15th 20th ... iterations after injection  
        max_iters = min(exp_data.base_itr, exp_data.inj_itr+20)
        for i in range(exp_data.inj_itr+5, max_iters+1,5):
            #inj_list[0,1,2] --> [inj+5,inj+10,inj+20]  
            if i == exp_data.inj_itr+15:
                continue;
            else:
                if i == exp_data.inj_itr+5:
                    index = 0
                elif i == exp_data.inj_itr+10:
                    index = 1
                else: # i == exp_data.inj_itr+20:
                    index = 2
            #check difference
                name_m = vec + str(i-exp_data.inj_itr)+"m"
                name_i = vec + str(i-exp_data.inj_itr)+"i"
                min_err, min_point =  check_vector(i, injlist[index], baselist,'r', exp_data.base_itr)
                exp_data.margins[name_m] = min_err
                exp_data.conviters[name_i] = min_point

    #create data frame                                                                                                                                              
    data_dict = get_dict(exp_data)
    #print data_dict
    dt = pd.DataFrame.from_dict(data_dict, orient='index').transpose() 
    
    #open ml model
    #pkl name? input from c++?
    pkl_location = models_home + sol + "/" + pkl_name
    ml_model = pickle.load(open(pkl_location, 'rb'))

    #get prediction
    result = ml_model.predict(dt)
    return result[0]

#end predict
        
        

