cd $SRC_HOME
mkdir results
cd results/
mkdir multibit
cd multibit
mkdir simulation-baseline
cd simulation-baseline
#for detector in "aid" "mad" "newsum" "orth" "ml"
for detector in "ml-60" "ml-70" "ml-80"
do
    mkdir $detector
    cd $detector
    
    if [ $detector == "aid" ];
    then
	for aid_param in "0.00078125"
	do
	    mkdir $aid_param
	    echo $aid_param
	    cd $aid_param
	    
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
	done
    elif [ $detector == "mad" ];
    then
        for mad_param in "0.1"
        do
            mkdir $mad_param
            cd $mad_param
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
        done
    elif [ $detector == "newsum" ];
    then
        for newsum_param in "0.0000000001"
        do
            mkdir $newsum_param
            cd $newsum_param
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
        done
    elif [ $detector == "orth" ];
    then
        for orth_param in "0.0000000001"
        do
            mkdir $orth_param
            cd $orth_param
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
        done

    elif [ $detector == "ml-50" ] || [ $detector == "ml-60" ] || [ $detector == "ml-70" ] || [ $detector == "ml-80" ];
    then
        for ml_param in "normal_numDS1" "normal_numDS4" "normal_numDS8" "normal_numDS16" "normal_numDS28"
        do
            mkdir $ml_param
            cd $ml_param
            for solver in "cg" "cgs" "bicg" "bicgsta" "iccg"
            do
                mkdir $solver
                cd $solver
                for ds in "af_shell3" "af_shell4" "af_shell7" "af_shell8" "bcsstk13" "bcsstk14" "bcsstk15" "bcsstk16" "bcsstk24" "bcsstk27" "bcsstk28" "bcsstk38" "ex3" "ex9" "ex13" "ex15" "\
Kuu" "msc04515" "nasa2146" "Pres_Poisson" "sts4098" "s1rmq4m1" "s2rmq4m1" "s3rmq4m1" "s1rmt3m1" "s2rmt3m1" "s3rmt3m1" "s3rmt3m3"
                do
                    mkdir $ds
                done
                cd ../
            done
            cd ../
        done
    fi

    

    cd ../
done
cd ../
