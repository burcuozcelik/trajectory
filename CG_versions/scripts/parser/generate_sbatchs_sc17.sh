for training in `ls data/datasets/training/*.dat`
do 
    ftraining=`echo $training | awk -F '/' '{print $4}'`
    ftraining=`echo $ftraining | awk -F '.dat' '{print $1}'`
    training="../data/datasets/training/"$ftraining".dat"
    
    echo $ftraining

    #for testing in 
    ftesting=`echo $ftraining | sed 's/train/test/g'`
    testing="../data/datasets/testing/"$ftesting".dat"
    fresult=`echo $ftraining | sed 's/train_//g'`
    results="../model/"$fresult".out"

    #echo $ftesting
    sbatchf="./sbatchs/"$ftraining".sh"
    echo "#!/bin/bash" > $sbatchf
    echo "#SBATCH --partition=pal" >> $sbatchf
    echo "#SBATCH --account pal"  >> $sbatchf
    echo "#SBATCH -N 1" >> $sbatchf
    echo "#SBATCH -e $ftesting.err"   >> $sbatchf
    echo "#SBATCH -o /dev/null"  >> $sbatchf

    
    training="../data/datasets/training/"$ftraining".dat"
    ftesting=`echo $ftraining | sed 's/train/test/g'`
    testing="../data/datasets/testing/"$ftesting".dat"
    fresult=`echo $ftraining | sed 's/train_//g'`
    results="../model/"$fresult".out"
    echo "python ../classifiers.py $training $testing > $results" >> $sbatchf
 done
