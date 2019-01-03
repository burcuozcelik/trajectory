#!/bin/bash

SAMPLES=$1
INFILE=$2
OUTFILE=$3
NLINES=`wc -l $INFILE | awk -F ' ' '{print$1}'`

echo "Generating $SAMPLES samples fomr file $INFILE ($NLINES lines)..."

for i in `seq 1 $SAMPLES`
do
    N=`shuf -i 1-$NLINES -n 1`
#    echo $N
    eval "sed -n '$N""p' $INFILE" >> $OUTFILE
done
