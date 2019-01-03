#!/bin/bash
DIR="./sbatchs-datasets/train-"

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
                    sbatchname=$DIR$solver-$chunksize-$dataset-$training-$testing
                    echo "#!/bin/bash" > $sbatchname.sh
                    echo "#SBATCH --partition=pal" >> $sbatchname.sh
                    echo "#SBATCH --account pal">> $sbatchname.sh
                    echo "#SBATCH -N 1">> $sbatchname.sh
                    echo "#SBATCH -e ./job-"$solver"-"$chunksize"-"$dataset"-"$training"-"$testing".err">> $sbatchname.sh
                    echo "#SBATCH -o ./job-"$solver"-"$chunksize"-"$dataset"-"$training"-"$testing".out" >> $sbatchname.sh
                    echo "cd ../" >> $sbatchname.sh
                    echo "python collectDataset_for_classifier_chunkn_shuffle.py " $solver $chunksize "../results/" $dataset $training $testing >> $sbatchname.sh
                done
            done
        done
    done
done


#for i in 1 2 3 4 5
#do
#    for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
#    do
#	for j in 4 8 16 28
#	do
#            for k in 10 100 500
#	    do 
#		for n in 10 20 40
#	        do
#		    echo $solver-$n-$j-$k-$i.sh
#		    echo "#!/bin/bash" > $DIR$solver-$n-$j-$k-$i.sh
#		    echo "#SBATCH --partition=pal" >> $DIR$solver-$n-$j-$k-$i.sh
#		    echo "#SBATCH --account pal">> $DIR$solver-$n-$j-$k-$i.sh
#		    echo "#SBATCH -N 1">> $DIR$solver-$n-$j-$k-$i.sh
#		    echo "#SBATCH -e ./job-"$solver"-"$n"-"$j"-"$k"-"$i".err">> $DIR$solver-$n-$j-$k-$i.sh
#		    echo "#SBATCH -o /dev/null">> $DIR$solver-$n-$j-$k-$i.sh
#		    echo "cd ../" >> $DIR$solver-$n-$j-$k-$i.sh
#		    echo "mkdir data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i >> $DIR$solver-$n-$j-$k-$i.sh
 # 		    echo "python collectDataset_for_classifier_chunkn_parametric.py " $solver $n "../results/" $j $k $i >> $DIR$solver-$n-$j-$k-$i.sh
#		done
#	    done
#	done
#    done
#done
