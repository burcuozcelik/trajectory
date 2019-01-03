function [rdata] = data_load_composite_wbml_per_tsize(wdir,feature_type, resnum, feature_version, training_class, label_type, tested_type, solver, training_sizes, wbml)
   for i=1:length(training_sizes)
	clear data
	fnamewbl = sprintf("%s/%s_%s%s_%s_%s_%s_%s_%s.out", wdir, solver, feature_type, resnum, feature_version, training_class, label_type, tested_type, training_sizes{i});
	data(:,:)=dlmread(fnamewbl);
        lindex=wbml(i);
        rdata(i,:) = [data(lindex,4) data(lindex,5)];
   endfor
   geoprecision=mean(rdata(:,1), 'g');
   georecall=mean(rdata(:,2), 'g');
   rdata(i+1,:) = [geoprecision georecall];
endfunction

