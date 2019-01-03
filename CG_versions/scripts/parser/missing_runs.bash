for i in `ls *last*.sh `
  do
  name=`echo $i | awk -F '.sh' '{print $1}'`
  output=$( cat ../data/model/$name.out | wc -l)
  if [ $output == "0" ];
  then
    echo $i
    sbatch $i
    sleep 2
  fi
  done
