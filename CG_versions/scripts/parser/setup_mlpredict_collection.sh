cd $SRC_HOME/scripts/parser
mkdir sbatchs_multi
cd sbatchs_multi

mkdir mlpredict
cd mlpredict
for solver in "cg"
do
    ln -s $SRC_HOME/src/global/$solver"_mlpredict" .
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


