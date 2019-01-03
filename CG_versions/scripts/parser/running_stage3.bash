for i in `cat missing_stage3`
do
    numex=`squeue -lp pal | wc -l`
    #echo $numex
    while [ $numex -gt 150 ]
    do
	numex=`squeue -lp pal | wc -l`
	sleep 2
    done
    
    echo "submitting"
    fname=`echo $i | awk -F '-' '{print $2}'`
    dataset=`echo $fname | awk -F '_' '{print $1}'`
    chunksize=`echo $fname | awk -F '_' '{print $2}'`
    training=`echo $fname | awk -F '_' '{print $3}'`
    testing=`echo $fname | awk -F '_' '{print $4}'`
    app=`echo $fname | awk -F '_' '{print $5}'`
    dir=$app"_"$dataset"_"$chunksize"_"$training"_"$testing
    echo $dir
    cd $dir"/"
    sbatch $i
    cd ../
done



