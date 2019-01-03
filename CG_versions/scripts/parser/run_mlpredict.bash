DIR=""$SRC_HOME"results/mlpredict/"
cdir=`pwd`
where="results"
nrum_in_each_sbatch=25
total_run_index=$((nrum_in_each_sbatch-1))

while true;
do
    num_sbatch_submitted=0
    for i in `ls *.sh`
    do
	numex=`squeue -lp pal | wc -l`
	while [ $numex -gt 150 ]
	do
	    numex=`squeue -lp pal | wc -l`
	    sleep 2
	done
	echo $i
	fname=`echo $i | awk -F '\\\.sh' '{print $1}'`
	if [ "`echo $fname | awk -F '_' '{print $8}'`" = "" ];
	then
	    solver=`echo $fname | awk -F '_' '{print $1}'`
	    dataset=`echo $fname | awk -F '_' '{print $2}'`
	    dis=`echo $fname | awk -F '_' '{print $3}'`
	    bf=`echo $fname | awk -F '_' '{print $4}'` 
	    index=`echo $fname | awk -F '_' '{print $5}'` 
	    stmt=`echo $fname | awk -F '_' '{print $6}'`
	    vec=`echo $fname | awk -F '_' '{print $7}'`
	else
	    solver=`echo $fname | awk -F '_' '{print $1}'`
	    dataset=`echo $fname | awk -F '_' '{print $2}'`"_"`echo $fname | awk -F '_' '{print $3}'`
	    dis=`echo $fname | awk -F '_' '{print $4}'`
	    bf=`echo $fname | awk -F '_' '{print $5}'` 
	    index=`echo $fname | awk -F '_' '{print $6}'` 
	    stmt=`echo $fname | awk -F '_' '{print $7}'`
	    vec=`echo $fname | awk -F '_' '{print $8}'`
	fi
	
	isrun="0"
	for j in `seq 0 $total_run_index`
	do
	    cfile=$DIR$where"/"$solver"/"$dataset"/"$solver"_"$dataset"_"$dis"_"$bf"_"$index"_"$stmt"_"$vec"_"$j".dat"
	    echo $cfile
	    if [ -f $cfile ];
	    then
		lline=$( grep -re "Last iteration found" $cfile | wc -l )
		
		if [ $where == "data" ];
		then
		    cdata=$( grep -re "x_vector" $cfile | wc -l )
		    
		##just check
		    if [[ $cdata == "0" && $lline == "1" ]];
		    then
                    echo "DID NOT COLLECT DATA $cfile"
		    fi
		    
		    if [ $cdata == "0" ];
		    then
			isrun="1"
			break
		    fi
		    
		elif [ $where == "baseline" ];
		then
		    if [ $lline == "0" ];
		    then
			isrun="1"
			break
		    fi
	    fi
		
	    else
		isrun="1"
	    fi
	done
	
	if [ $isrun == "1" ];
	then
	    echo "submitting $i"
	    num_sbatch_submitted=$((num_sbatch_submitted+1))
	    sbatch $i
	    sleep 2
	fi
    done

    echo "Number of sbatches submitted $num_sbatch_submitted"
    if  [ $num_sbatch_submitted == 0 ];
    then
	echo "Completed"
	break
    fi
done

