DIR=""$SRC_HOME"results/mlpredict/"
cdir=`pwd`
where="results"
nrum_in_each_sbatch=25
total_run_index=$((nrum_in_each_sbatch-1))


for i in `ls *.sh`
do
    echo $i
    $i
    sleep 2
done

