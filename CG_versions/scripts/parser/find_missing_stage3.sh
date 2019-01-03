#!/bin/bash

rm missing_stage3
itype="anomaly"
output="/people/kest268/src/projects/ADEM/composition/CG_versions/scripts/results/ml/"

for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
#for solver in "cg"
do
    #for dataset in 4 8 16 28
    for dataset in 28 16
    do
        #for chunksize in 10 40
	for chunksize in 10
        do
            #for training in 8000 16000
	    for training in 8000
            do
                for testing in 5000
                do
		    echo $solver"_"$dataset"_"$chunksize"_"$training"_"$testing
		    dir="./data/datasets/memory/"$itype"/"$solver"/"$dataset"_"$chunksize"_"$training"_"$testing/
                    model_file=$dir$dataset"_"$chunksize"_"$training"_"$testing".train.model"
		    if [ -f  $model_file ]
                    then
			for tf in `ls $dir$solver*.test`
			do
			    fname=`echo $tf | awk -F '/' '{print $8}'`
			    pred_file=$output$solver"/"$dataset"_"$chunksize"_"$training"_"$testing/$fname".pred"
			    if [  ! -f  $pred_file ]
			    then
				echo $fname
				echo "stage3-"$dataset"_"$chunksize"_"$training"_"$testing"_"$fname".sh" >> missing_stage3
			    fi
			done
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
#            for k in 10 100 500
#	    do 
#		for n in 10 20 40
#	        do
#		    for ctype in "woDetectors" 
#		    do
#			if [ -f "data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/*woDetectors*.test2.model" ]
#                        then
#			    echo 
			    #for tfile in `ls data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/*woDetectors*.test2`
                            #do
			#	fname=`echo $tfile | awk -F '/' '{print $6}'`
				
			#	predfile=$( cat $tfile".pred" | wc -l )
			#	if [ $predfile == "0" ]
			#	then
			#	    echo "$solver-$n-$j-$k-$i/stage3-$fname" >> missing_stage3
			#	fi
                        #
			 #   done
#			fi
#		    done
#		done
#	    done
#	done
#    done
#done
