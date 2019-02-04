DIR=""$SRC_HOME"results/multibit/"
cdir=`pwd`
where=`echo $cdir | awk -F '/' '{print $NF}'`
nrum_in_each_sbatch=25
total_run_index=$((nrum_in_each_sbatch-1))

num_sbatch_submitted=0
for i in "bcsstk28" "bcsstk38" "ex15" "Pres_Poisson" 
do
    numex=`ps | grep 'get_traj' | wc -l`
    while [ $numex -gt 4 ]
    do
#	echo "num of jobs $numex"
	sleep 10
	numex=`ps | grep 'get_traj' | wc -l`
    done
    
    echo "submitting $i"
    num_sbatch_submitted=$((num_sbatch_submitted+1))
    python get_trajectory_json.py bicg $i &
    sleep 2
    
done

echo "Number of sbatches submitted $num_sbatch_submitted"


