function rdata = data_load_composite_wTsize(wdir,resnum,solver,training_class,training_size,bml)
  for i=1:length(training_size)
        clear data
	fname = sprintf("%s/%s_%s_%s_%s.out", wdir, solver, resnum, training_class, training_size{i});
	%fname
	data(:,:)=dlmread(fname);
        rdata(i,:) = [data(bml,4) data(bml,5)];
  endfor
endfunction
