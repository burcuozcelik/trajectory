function [rdata,bestparams] = data_load_detectors(wdir, detector,solvers, params, itype)
  for i=1:length(solvers)
	  bestparam=0.0;
          bestfscore=0.0;
          bestprecision=0.0;
          bestrecall=0.0;
	  for j=1:length(params)
	        fname = sprintf("%s/%s_%s_%s_%s_ad_extra.dat", wdir, detector,solvers{i}, params{j},itype);
                %data(j,:)=dlmread(fname);
                data=dlmread(fname);
               
                if (data(1,8) > bestfscore)
                  bestfscore=data(1,8);
                  bestprecision=data(1,6);
                  bestrecall=data(1,7);
                  bestparam=params{j}; 
                endif
          endfor
	  %sdata=sortrows(data,-8);
          %rdata(i,:) = [sdata(1,6) sdata(1,7)];
          %bestprecision
	  %bestrecall
	  %bestfscore
	  %bestparam
	  %x={ bestprecision bestrecall bestfscore bestparam };
          rdata(i,:) = [bestprecision bestrecall];
	  bestparams{i} = bestparam;
  endfor
endfunction
