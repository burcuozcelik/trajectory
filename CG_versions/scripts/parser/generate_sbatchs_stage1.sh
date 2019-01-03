#!/bin/bash
DIR="./sbatchs-stage1/stage1-"

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
			echo $solver-$n-$j-$k-$i-$ctype.sh
			echo "#!/bin/bash" > $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "#SBATCH --partition=pal" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "#SBATCH --account pal">> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "#SBATCH -N 1">> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "#SBATCH -e ./job-"$solver"-"$n"-"$j"-"$k"-"$i"-"$ctype".err">> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "#SBATCH -o /dev/null">> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "cd ../" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "cp classifiers_wstages.py data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "cp timingModule.so data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			echo "cd data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
  			echo "python classifiers_wstages.py -s 1 -t "$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype".train1 -x "$solver"_"$n"_"$j"_"$k"_"$i"_"$ctype".test1" >> $DIR$solver-$n-$j-$k-$i-$ctype.sh
			#DIRECTORY="data/datasets/memory/"$solver"/"$n"_"$j"_"$k"_"$i"/"
			#if [ -d "$DIRECTORY" ]; then
			#    echo "$solver-$n-$j-$k-$i-$ctype.sh" >> run_list.stage1
			#else
			#    echo "$solver-$n-$j-$k-$i.sh" >> missing_training.stage1
                        #fi
		    done
		done
	    done
	done
    done
done
