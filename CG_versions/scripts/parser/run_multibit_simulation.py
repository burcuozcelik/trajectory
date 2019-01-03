import os
import glob
from commands import *

detectors = ['mad', 'newsum', 'orth']

def run_scripts():
    global launched_jobs
    launched_jobs=[]
    num_submitted_jobs = 0
    for sname in sbatches:
        #script_name=sfile.split("/")[-1]
        detector=sname.split("_")[0]
       
        if not detector in detectors:
            continue
        
        status, num_jobs = getstatusoutput('squeue -p pal  | grep -re kest268 | wc -l')
        while int(num_jobs) > 150:
            status, text = getstatusoutput('sleep 2')
            status, num_jobs = getstatusoutput('squeue -p pal  | grep -re kest268 | wc -l')
        
        with open(sname, 'rb') as f:
            lines = f.readlines()
            tlines= len(lines)

            for findex in range(0, tlines,2):
                if "python" in lines[findex]:
                    #print "\n" + lines[findex] + "\n"
                    
                    foutname= lines[findex].split(">")[-1]
                    foutname=foutname.split(" ")[1]

                    is_run=1
                    if os.path.exists(foutname):
                        fout=open(foutname,'rb')
                        fout_lines = fout.readlines()
                        
                        if fout_lines:
                            last_line = fout_lines[-1]
                            if "Last iteration found" in last_line:
                                is_run = 0

                    if is_run == 1:
                        cmd= "sbatch " + sname
                        print "Running ", cmd
                        launched_jobs.append(sname)
                        status, jsubmitted = getstatusoutput(cmd)
                        print jsubmitted
                        num_submitted_jobs = num_submitted_jobs + 1
                        break

    return num_submitted_jobs

    
def main():
    global sbatches
    #sbatches = glob.glob("*.sh")
    #print "Total number of sbatch files: ", len(sbatches)

    rtimes=0
    job_history = ["-1","-1","-1","-1","-1"]
    enter_loop = 1

    while enter_loop:
        sbatches = glob.glob("*.sh")
        print "Total number of sbatch files: ", len(sbatches)

        rrun_number=run_scripts()
        print "# of experiments are launched: ", rrun_number
        job_history[rtimes % len(job_history)] = str(rrun_number)
       
        if rrun_number == 0:
            enter_loop = 0
            print "GOOD: All the experiments completed"
        else:
            status, num_jobs = getstatusoutput('squeue -p pal  | grep -re kest268 | wc -l')
            while int(num_jobs) != 0:
                status, text = getstatusoutput('sleep 2')
                status, num_jobs = getstatusoutput('squeue -p pal  | grep -re kest268 | wc -l')
        
        enter_loop =0
        for njob in job_history:
            if str(rrun_number) != njob:
                enter_loop = 1

        if enter_loop == 0:
            msg= "BAD: Last " + str(len(job_history)) + " times, there are always " + str(rrun_number) + " experiments missing"
            print msg
            flaunched = open("launced_jobs.out", "w")
            flaunched.write("Total number:" + str(rrun_number) + "\n")
            for jfile in launched_jobs:
                flaunched.write(jfile + "\n")

        rtimes = rtimes + 1
main()
