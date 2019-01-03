import os
import sys
from sys import argv


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


if len(argv) < 3:
        print("Usage: Solver Dataset")
        sys.exit(1)

sol = argv[1]
ds= argv[2]

cg_home= "/pic/projects/pal/mutl832/"
dirname = cg_home + "convergence/"  
exp_list = {}
interval = 5
dsmap = {}

############# COLUMNS #############
# Injected Error -- Observed Itr -- Observed Vec -- Observed Err -- Injection Itr -- Final Result -- FileName -- Injected Vec 
#                      10        --     p        --    err
#                      10        --     x        --    err
#                      10        --     r        --    err
#                      20        --     p        --    err  

def isclose(a, b, rel_tol=1e-30, abs_tol=0.0):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)
#end isclose

def calculate_marginal_difference(avec,bvec):
    diff = 0
    for i in range(len(avec)):
        if isclose(float(avec[i]),float(bvec[i])):
	    continue
        elif isclose(float(avec[i]),0.0):
            if isclose(float(bvec[i]),0.0):
                continue
            else: 
   #             print "a: ",float(avec[i]), "  b:", float(bvec[i])
   #             print "diff at ", i
                diff += abs((float(bvec[i])-float(avec[i]))/float(bvec[i]))
        else:
   #         print "a: ",float(avec[i]), "  b:", float(bvec[i])
   #         print "diff at ", i
            diff += abs((float(avec[i])-float(bvec[i]))/float(avec[i]))
   #     print "d: ", diff
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
    len(veclist)
#end  add_vecline_to_list

def GetVecData(vec,fname,shortname):
    global exp_list
    datalist = []
    injvectorlist = ["p","r","x"]
    linename = vec + "_vector"
    pars = ExpParams()
    with open(fname) as datafile:
        line = datafile.readline()
        while line:
            if line.startswith(linename):
                add_vecline_to_list(line,datalist)
	    elif "data" in fname:
            #check if we already have the data from dictionary
                if not shortname in exp_list:
                    #if we don't have it in the dictionary, get the data now
		    if line.startswith("Baseline iterations: "):
                        pars.base_itr = int(line[len("Baseline iterations: "):])
		    elif line.startswith("Place in iterations space (when):"):
                        pars.inj_itr = int(line[len("Place in iterations space (when): "):])
		    elif line.startswith("Place among vectors: "):
                        pars.inj_vec = injvectorlist[int(line[len("Place among vectors: "):])]
		    elif line.startswith("Last iteration found: "):
                        pars.tot_itr = int(line[len("Last iteration found: "):])
            line = datafile.readline()
    if not shortname in exp_list:
        pars = get_final_data(pars,shortname)
        exp_list[shortname] = pars
    return datalist
#end GetVecData

def load_map_data():
    global dsmap
    content =""
    outmap = os.environ["SRC_HOME"] + "results/solver_json_results/maps/" + sol + "_" + ds + "_sorted.map"
    with open(outmap, 'r') as m:
        content = m.read()
    pairs = content.split('&')
    for i in range(0,len(pairs)-1):
        vals = pairs[i].split(';')
        if len(vals)>1:
            dsmap[vals[0]] = vals[1]
#end load_map_data

def get_final_data(pars,f):
    #for CG
    fname = f.replace('bit_5_2','bit_5_3')
    fname = fname.replace('bit_6_1','bit_6_2')
    k=""
    if "5_3" in fname:
        k = "5_3"
    elif "6_1" in fname:
        k = "6_1"
    elif "3_0" in fname:
        k = "3_0"
    else:
        return pars
    #end for CG
    
    mapobj = dsmap[k]
    lines = mapobj.split('\n')
    dataline = ""
    for l in lines:
        if fname in l:
            dataline = l
            break
    dataarr = dataline.split(',')
    for dt in dataarr:
        if "masked" in dt:
            pars.passed = dt[-1]
        if "activation"in dt:
            pars.activation = int(dt.split(':')[1])
        if "tot_itr" in dt:
            pars.final_itr =int(dt.split(':')[1])
    pars.ref_name = fname
    return pars
#end get_final_data

def main():
    #first load normal injection result map from file to dsmap
    load_map_data()

    vecs = ["r","p","x"]
    res_text = ""
    folder = cg_home + "convergence/data/" + sol + "/" + ds + "/"
    filelist = os.listdir(folder)
    basename = dirname + "baseline/" + sol + "/" + ds + "/" + sol + "_" + ds + "_baseline.dat"
    filename = "./" + sol + "_" + ds + "_combined"
    #get error margins for each vector separately for memory consumption
    for vec in vecs:
    #    global exp_list
    #    exp_list = {}
        res_text = ""
        baselist = GetVecData(vec,basename,"baseline")
        for f in filelist:#for each injection file:
            fname = os.path.join(folder,f)
            injlist = GetVecData(vec,fname,f)
            inj_itr = exp_list[f].inj_itr
            if(exp_list[f].inj_vec == vec):
                exp_list[f].err_margin = calculate_marginal_difference(baselist[inj_itr-1],injlist[inj_itr-1])
            ##compare injlist and baselist for 10th 15th 20th ... iterations after injection
            max_iters = min(exp_list[f].base_itr, exp_list[f].tot_itr)
            for i in range(inj_itr+5, max_iters,5):
                #check difference
                name_m = vec + str(i-inj_itr)+"m"
                name_i = vec + str(i-inj_itr)+"i"
                min_err, min_point =  check_vector(i, injlist[i], baselist,'r', max_iters)
                exp_list[f].margins[name_m] = min_err
                exp_list[f].conviters[name_i] = min_point

    final_text = ""
    for key,val in exp_list.items():
#        print val.get_longer_json(key)
#        final_text += val.get_combined_json() + '\n'
        final_text += val.get_longer_json(key) + '\n'
    filen = filename + ".json"
    with open(filen,"w") as outf:
        outf.write(final_text)

#end main

############################

main()
                 

                
