THRESHOLD_AID=(0.00078125 0.000001 0.00001 0.0001 0.001 0.01) #impact error bound                                                                                                           
THRESHOLD_AID_NAME=(0.00078125 0.000001 0.00001 0.0001 0.001 0.01)

THRESHOLD_SSD=(0.00078125 0.000001 0.00001 0.0001 0.001 0.01) #impact error bound                                                                                                           
THRESHOLD_SSD_NAME=(0.00078125 0.000001 0.00001 0.0001 0.001 0.01)

THRESHOLD_NEWSUM=(0.0000000001 0.000000001 0.00000001 0.0000001) #floating point precision                                                                                                  
THRESHOLD_NEWSUM_NAME=(1e-010 1e-09 1e-08 1e-07) #floating point precision                                                                                                                    

THRESHOLD_GAGAN=(1) #tuned window size and #threshould                                                                                                                                        

THRESHOLD_ORTH=(0.0000000001 0.000000001 0.00000001 0.0000001) #floating point precision                                                                                                    
THRESHOLD_ORTH_NAME=(1e-010 1e-09 1e-08 1e-07) #floating point precision                                                                                                                      

DATASETS=(af_shell3 af_shell4 af_shell7 af_shell8 aft01 bcsstk13 bcsstk14 bcsstk15 bcsstk16 bcsstk24 bcsstk27 bcsstk28 bcsstk38 crystm01 ex13 ex15 ex3 ex9 Kuu msc04515 Muu nasa2146 Pres_Poisson s1rmq4m1 s1rmt3m1 s2rmq4m1 s2rmt3m1 s3rmq4m1 s3rmt3m1 s3rmt3m3 sts4098)
PMAG=(0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62\
 63)
PTIME=(0 1 2 3)
PPOS=(0.2 0.4 0.6 0.8)

detector=$1

output="$detector.csv"
output_per="$detector.csv"

rm $output
rm $output_per

wvalue="-"" ""-"
wout="-"" ""-"

for (( k=0; k<${#VALUES[@]}; k++ ))
do
    wvalue=$wvalue" "${VALUES[$k]}" "${VALUES[$k]}" "${VALUES[$k]}
    wout=$wout" ""FalsePos"" ""FalseNeg"" ""TruePos"
done

echo $wvalue >> $output_per
echo $wvalue >> $output

echo $wout >> $output_per
echo $wout >> $output



for (( i=0; i<${#bits[@]}; i++ ))
do
    for (( j=0; j<${#APPS[@]}; j++ ))
    do
	line=""
	line_per=""
	for (( k=0; k<${#VALUES[@]}; k++ ))
	do
	    falsepositive=0
            falsenegative=0
            truepositive=0
	    for (( l=0; l<${#DATASETS[@]}; l++ ))
            do
                aid=`grep -re "First error detected" ${APPS[$j]}_aid_${DATASETS[$l]}_${VALUES[$k]}_2.${bits[$i]}.out | wc -l`
                default=`grep -re "Iteration Check:" ../../default/${APPS[$j]}_default_${DATASETS[$l]}_2.${bits[$i]}.out | awk -F ' ' '{print $3}'`
		if [ "$default" == "PASSED" ]; then 
		    if [ $aid -ne 0 ]; then
			falsepositive=$((falsepositive+1))
		    fi
		else
                    if [ $aid -ne 0 ]; then
			truepositive=$((truepositive+1))  
		    else
			falsenegative=$((falsenegative+1))    
                    fi
		fi
	    done
	    #if [ ${bits[$i]} -eq 0 ]; then
	    #	numinj={#DATASETS[@]}
	    #else
	#	numinj=`grep -re "FAILED" ../../default/${APPS[$j]}_default_*_2.${bits[$i]}.out | wc -l`
	 #   fi
	    numinj=${#DATASETS[@]}
	    line=$line" "$falsepositive" "$falsenegative" "$truepositive
	    line_per=$line_per" ""="$falsepositive"*100/"$numinj" ""="$falsenegative"*100/"$numinj" ""="$truepositive"*100/"$numinj
	done
	echo ${APPS[$j]}" "$line_per >> parse_aid_output_per.csv
	echo ${APPS[$j]}" "$line >> parse_aid_output.csv
    done
    echo "" >> parse_aid_output.csv
    echo "" >> parse_aid_output_per.csv
done

