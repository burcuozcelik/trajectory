#!/bin/bash
DIR="./sbatchs-stage1-2/stage1-2-"

for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
do
    for dataset in 4 8 16 28
    do
        for chunksize in 10 40
        do
            for training in 8000 1600 16000
            do
                for testing in 5000
                do
                    sbatchname=$DIR$solver-$chunksize-$dataset-$training-$testing
                    echo "#!/bin/bash" > $sbatchname.sh
                    echo "#SBATCH --partition=pal" >> $sbatchname.sh
                    echo "#SBATCH --account pal">> $sbatchname.sh
                    echo "#SBATCH -N 1">> $sbatchname.sh
                    echo "#SBATCH -e ./job-"$solver"-"$chunksize"-"$dataset"-"$training"-"$testing".err">> $sbatchname.sh
                    echo "#SBATCH -o ./job-"$solver"-"$chunksize"-"$dataset"-"$training"-"$testing".out" >> $sbatchname.sh
                    echo "cd ../" >> $sbatchname.sh
                    echo "python collectDataset_for_classifier_chunkn_shuffle.py " $solver $chunksize "../results/" $dataset $training $testing >> $sbatchname.sh
		    mvdir="./data/datasets/memory/anomaly/"$solver"/"$dataset"_"$chunksize"_"$training"_"$testing/
                    echo "cp classifiers_wstages.py $mvdir" >> $sbatchname.sh
                    echo "cp timingModule.so $mvdir" >> $sbatchname.sh
                    echo "cd $mvdir" >> $sbatchname.sh
                    echo "python classifiers_wstages.py -s 1 -t "$dataset"_"$chunksize"_"$training"_"$testing".train -x "$dataset"_"$chunksize"_"$training"_"$testing".test" >> $sbatchname.sh
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
#		    #for ctype in "wDetectors" "woDetectors"
#		    for ctype in "woDetectors" 
#		    do
#			echo $solver-$n-$j-$k-$i-$ctype.sh
#			echo "#!/bin/bash" > $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "#SBATCH --partition=pal" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "#SBATCH --account pal">> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "#SBATCH -N 1">> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "#SBATCH -e ./job-"$solver"-"$n"-"$j"-"$k"-"$i"-"$ctype".err">> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "#SBATCH -o /dev/null">> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "cd ../" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "cp classifiers_wstages.py data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "cp timingModule.so data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#			echo "cd data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
 # 			echo "python classifiers_wstages.py -s 1 -t "$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype".train1 -x "$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype".test1" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh

#			echo "python classifiers_wstages.py -s 2 -t "$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype".train2 -a "$solve\r"_"$n"_"$j"_"$k"_"$i"_"$ctype".train1.best" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
#		    done
#		done
#	    done
#	done
#    done
#done
