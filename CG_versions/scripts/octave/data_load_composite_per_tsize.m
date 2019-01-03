function [rdata,bml] = data_load_composite_per_tsize(wdir,feature_type, resnum, feature_version, training_class, label_type, tested_type, solver, training_size)
for i=1:length(training_size)
	fname = sprintf("%s/%s_%s%s_%s_%s_%s_%s_%s.out", wdir, solver, feature_type, resnum, feature_version, training_class, label_type, tested_type, training_size{i});
	data(:,:)=dlmread(fname);
        sdata=sortrows(data,-2);
        rdata(i,:) = [sdata(1,4) sdata(1,5)];
        bml(i)=sdata(1,1);
   endfor
endfunction
  
