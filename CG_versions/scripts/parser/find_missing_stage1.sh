#!/bin/bash

rm missing_stage1


for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
do
    for dataset in 4 8 16 28
    do
        for chunksize in 10 40
        do
            for training in 8000 1600
            do
                for testing in 5000
                do
		    fname="job-"$solver-$chunksize-$dataset-$training-$testing
		    output_training=$( grep -re "Completed!"  ./sbatchs-datasets/$fname | wc -l )
		    if [ $output_training == "1" ]
		    then
			dir="data/datasets/memory/anamoly/"$solver"/"$dataset"_"$chunksize"_"$training"_"$testing/
			output_model=$(cat $dir$solver"_"$dataset"_"$chunksize"_"$training"_"$testing".model" | wc -l ) 
			if [ $output_model != "0" ]
			then
			    echo "stage1-$solver-$chunksize-$dataset-$training-$testing.sh" >> missing_stage1
			fi
		    fi
		done
	    done
	done
    done
done

#for i in 1
#do
#    for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
#    do
#	for j in 4 8 16 28
#	do
#           for k in 10 100 500
#	    do 
#		for n in 10 20 40
#	        do
#		    #for ctype in "wDetectors" "woDetectors"
#		    for ctype in "woDetectors"     
#		    do
#			#echo data/datasets/memory/$solver/$n"_"$j"_"$k"_"$i/$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype.train1.best
#			output=$( cat "data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i/$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype".train1.best" | wc -l )
#			#echo $output
#			if [ $output == "0" ]
#			then
#			    echo "stage1-$solver-$n-$j-$k-$i-$ctype.sh" >> missing_stage1
#                       fi
#		    done
#		done
#	    done
#	done
#    done
#done
