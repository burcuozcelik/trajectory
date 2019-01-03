#!/bin/bash

rm missing_stage2

for i in 1
do
    for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
    do
	for j in 4 8 16 28
	do
            for k in 10 100 500
	    do 
		for n in 10 20 40
	        do
		    #for ctype in "wDetectors" "woDetectors"
		    for ctype in "woDetectors" 
		    do
			#echo data/datasets/memory/$solver/$n"_"$j"_"$k"_"$i/$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype.train1.best
			output_best=$( cat "data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i/$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype".train1.best" | wc -l )
			output_model=$( cat "data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i/$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype".train2.model" | wc -l )
			#echo $output
			if [ $output_best == "1" ]
			then
			    if [ $output_model == "0" ]
			    then
				echo "stage2-$solver-$n-$j-$k-$i-$ctype.sh" >> missing_stage2
			    fi
                        fi
		    done
		done
	    done
	done
    done
done
