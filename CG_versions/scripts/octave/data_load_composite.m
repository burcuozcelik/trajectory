function [rdata,bml] = data_load_composite(wdir,feature_type, resnum, feature_version, training_class, label_type, tested_type, training_size, solvers)
for i=1:length(solvers)
	fname = sprintf("%s/%s_%s%s_%s_%s_%s_%s_%s.out", wdir, solvers{i}, feature_type, resnum, feature_version, training_class, label_type, tested_type, training_size);
	data(:,:)=dlmread(fname);
        sdata=sortrows(data,-2);
        rdata(i,:) = [sdata(1,4) sdata(1,5)];
        bml(i)=sdata(1,1);
   endfor
endfunction
  
