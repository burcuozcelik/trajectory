#!/bin/bash
DIR="./sbatchs-stage3/"
itype="anomaly"
output="/people/kest268/src/projects/ADEM/composition/CG_versions/scripts/results/ml/"

for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
do
    #for dataset in 4 8 16 28
    for dataset in 16
    do
        #for chunksize in 10 40
	for chunksize in 10
        do
            #for training in 8000 16000
	    for training in 8000
            do
                for testing in 5000
                do
		    mkdir $DIR$solver"_"$dataset"_"$chunksize"_"$training"_"$testing
		    basedir="data/datasets/memory/"$itype"/"$solver"/"$dataset"_"$chunksize"_"$training"_"$testing"/"
		    echo $solver"_"$dataset"_"$chunksize"_"$training"_"$testing
		    for tfile in `ls $basedir$solver*.test`
		    do
			fname=`echo $tfile | awk -F '/' '{print $7}'` 
			sname=$DIR$solver"_"$dataset"_"$chunksize"_"$training"_"$testing"/stage3-"$dataset"_"$chunksize"_"$training"_"$testing"_"$fname.sh
			echo "#!/bin/bash" > $sname
			echo "#SBATCH --partition=pal" >> $sname
			echo "#SBATCH --account pal">> $sname
			echo "#SBATCH -N 1">> $sname
			echo "#SBATCH -e ./job-"$fname >> $sname
			echo "#SBATCH -o /dev/null">> $sname
			echo "cd ../../" >> $sname
			echo "cp classifiers_wstages.py $basedir" >> $sname
			echo "cp timingModule.so $basedir" >> $sname
			echo "cd "$basedir >> $sname
  			echo "python classifiers_wstages.py -s 3 -x "$fname" -m "$dataset"_"$chunksize"_"$training"_"$testing".train.model" >> $sname
			echo "mkdir "$output$solver"/"$dataset"_"$chunksize"_"$training"_"$testing >> $sname
			echo "mv "$fname".pred "$output$solver"/"$dataset"_"$chunksize"_"$training"_"$testing >> $sname
		    done
		done
	    done
	done
    done
done
