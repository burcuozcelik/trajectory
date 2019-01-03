function [rdata] = data_load_composite_wbml(wdir,feature_type, resnum, feature_version, training_class, label_type, tested_type, training_size, solvers, wbml)
   for i=1:length(solvers)
	clear data
	fnamewbl = sprintf("%s/%s_%s%s_%s_%s_%s_%s_%s.out", wdir, solvers{i}, feature_type, resnum, feature_version, training_class, label_type, tested_type, training_size);
	data(:,:)=dlmread(fnamewbl);
        lindex=wbml(i);
	 
        %check this part
        %tsize=size(data,1)
	%if tsize < lindex
	%  sdata=sortrows(data,-2);
        %  rdata(i,:) = [sdata(1,4) sdata(1,5)];
	%else
	rdata(i,:) = [data(lindex,4) data(lindex,5)];
        % endif
   endfor
   geoprecision=mean(rdata(:,1), 'g');
   georecall=mean(rdata(:,2), 'g');
   rdata(i+1,:) = [geoprecision georecall];
endfunction
  
