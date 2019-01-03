#!/bin/bash

rm missing_stage_1_2

for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
do
    #for dataset in 4 8 16 28
    for dataset in 4 28   
    do
        #for chunksize in 10 40
	for chunksize in 10
        do
            #for training in 8000 1600 16000
	    for training in 8000
            do
                for testing in 5000
                do
                    fname="job-"$solver-$chunksize-$dataset-$training-$testing".out"
                    output_training=$( grep -re "Completed!"  ./sbatchs-stage1-2/$fname| wc -l )
                    if [ $output_training != "1" ]
                    then
			echo "stage1-2-$solver-$chunksize-$dataset-$training-$testing.sh" >> missing_stage_1_2
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
#		    DIRECTORY="data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/"
#		    if [ ! -d "$DIRECTORY" ]; then
#			echo "train-$solver-$n-$j-$k-$i.sh" >> missing_training
#                   fi
#		done
#	    done
#	done
#    done
#done
