cd $SRC_HOME
mkdir results
cd results/
mkdir multibit
cd multibit
mkdir baseline
cd baseline
for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
do
    mkdir $solver
    cd $solver
    for ds in "af_shell3" "af_shell4" "af_shell7" "af_shell8" "bcsstk13" "bcsstk14" "bcsstk15" "bcsstk16" "bcsstk24" "bcsstk27" "bcsstk28" "bcsstk38" "ex3" "ex9" "ex13" "ex15" "Kuu" "msc04515" "nasa2146" "Pres_Poisson" "sts4098" "s1rmq4m1" "s2rmq4m1" "s3rmq4m1" "s1rmt3m1" "s2rmt3m1" "s3rmt3m1" "s3rmt3m3"
    do
	mkdir $ds
    done
    cd ../
done
cd ../


mkdir data
cd data
for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
do
    mkdir $solver
    cd $solver
    for ds in "af_shell3" "af_shell4" "af_shell7" "af_shell8" "bcsstk13" "bcsstk14" "bcsstk15" "bcsstk16" "bcsstk24" "bcsstk27" "bcsstk28" "bcsstk38" "ex3" "ex9" "ex13" "ex15" "Kuu" "msc04515" "nasa2146" "Pres_Poisson" "sts4098" "s1rmq4m1" "s2rmq4m1" "s3rmq4m1" "s1rmt3m1" "s2rmt3m1" "s3rmt3m1" "s3rmt3m3"
    do
	mkdir $ds
    done
    cd ../
done
cd ../

cd $SRC_HOME/scripts/parser
mkdir sbatchs_multi
cd sbatchs_multi

mkdir baseline
cd baseline
for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
do
    ln -s $SRC_HOME/src/global/$solver $solver"_baseline"
    if [ ! -d datasets-all ]
    then
	ln -s $SRC_HOME/src/global/datasets-all/ .
    fi

    if [ ! -d iters ]
    then
	ln -s $SRC_HOME/src/global/iters/ .
    fi

    if [ ! -f run_multibit.bash ]
    then
        ln -s $SRC_HOME/scripts/parser/run_multibit.bash .
    fi

done
cd ../

mkdir data
cd data
for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
do
    ln -s $SRC_HOME/src/global/$solver"_collect" .
    if [ ! -d datasets-all ]
    then
	ln -s $SRC_HOME/src/global/datasets-all/ .
    fi

    if [ ! -d iters ]
    then
	ln -s $SRC_HOME/src/global/iters/ .
    fi

    if [ ! -f run_multibit.bash ]
    then
        ln -s $SRC_HOME/scripts/parser/run_multibit.bash .
    fi
done
cd ../
