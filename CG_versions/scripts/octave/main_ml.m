WDIR='../parser/data/model/'
composite_resnum={'10'};
composite_solvers={'cg' 'cgs' 'iccg' 'bicg' 'bicgsta' 'qmr'};
solver_bar_labels={'cg' 'cgs' 'iccg' 'bicg' 'bicgsta' 'qmr' 'avg'};
%composite_solvers={'cgs' 'qmr'};
composite_training_size={'1000'};

%changing
feature_type={'last'};
feature_version={'wDetectors' 'woDetectors'};
feature_version_name={'ML_composite' 'ML_only'};
label_type={'normal' 'dual'};
test_set={'all' 'sign' 'mantissa' 'exponent'};
%test_set={'mantissa'}

for i=1:length(feature_type)
        for j=1:length(feature_version)
		for k=1:length(label_type)
			composite_training_class={'1'};
                        [composite,bml]=data_load_composite(WDIR,feature_type{i},composite_resnum{1}, feature_version{j}, composite_training_class{1}, label_type{k}, 'all', composite_training_size{1}, composite_solvers);
                        %plot_bars(composite/100, composite_solvers, 'composite','Solvers');
                         for z=1:length(test_set)
			          composite_training_class={'2'};
                                  [composite_wbml]=data_load_composite_wbml(WDIR,feature_type{i},composite_resnum{1}, feature_version{j}, composite_training_class{1}, label_type{k}, test_set{z}, composite_training_size{1}, composite_solvers, bml); 
                                  fname = sprintf("%s_%s_%s_%s.eps", feature_version_name{j}, feature_type{i}, label_type{k}, test_set{z});
                                  %plot_bars(composite_wbml/100, solvers_bar_labels, fname,'Solvers');
                        endfor
                 endfor
       endfor
endfor

%%%effect of trainin_size
composite_training_size={'1000' '2000' '4000' '6000' '8000' '10000' '12000' '14000' '16000'};
for z=1:length(composite_solvers)
	for i=1:length(feature_type)
		for j=1:length(feature_version)
			for k=1:length(label_type)
				composite_training_class={'1'};
                                [composite_wtsize,bml_wtsize]=data_load_composite_per_tsize(WDIR,feature_type{i},composite_resnum{1}, feature_version{j}, composite_training_class{1}, label_type{k}, 'all' , composite_solvers{z}, composite_training_size);
                                bml_wtsize
                                for d=1:length(test_set)
					composite_training_class={'2'};
                                        [dData]=data_load_composite_wbml_per_tsize(WDIR,feature_type{i},composite_resnum{1}, feature_version{j}, composite_training_class{1}, label_type{k}, test_set{d}, composite_solvers{z}, composite_training_size, bml_wtsize);
                                        fname = sprintf("%s_%s_%s_%s_%s_wTsize.eps", composite_solvers{z}, feature_version_name{j}, feature_type{i}, label_type{k}, test_set{d});
                                        plot_bars(dData/100, composite_training_size, fname, 'Training Size');
                               endfor
                        endfor
                endfor
         endfor
endfor

%dData=data_load_composite_wTsize(WDIR,composite_resnum{1},composite_solvers{i}, composite_training_class{1},composite_training_size,bml(i));

