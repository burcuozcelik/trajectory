WDIR='../parser/data/detectors/memory/';
types={'sign' 'exponent' 'mantissa'};
x_labels={'cg' 'iccg' 'bicg' 'bicgsta' 'cgs' '' 'cg' 'iccg' 'bicg' 'bicgsta' 'cgs' '' 'cg' 'iccg' 'bicg' 'bicgsta' 'cgs'};
aid_together=[];
ssd_together=[];
newsum_together=[];
gagan_together=[];
orth_together=[];

aid_params={'0.01' '0.001' '0.0001' '0.00001' '0.000001' '0.00078125'};
aid_solvers={'cg' 'iccg' 'bicg' 'bicgsta' 'cgs'};
for i=1:length(types)
  [aid_all,aid_bests_all]=data_load_detectors(WDIR,"aid",aid_solvers,aid_params,types{i});
  aid_together = [aid_together ; aid_all; 0 0]; 
  %fname=strcat ("aid_only_", types{i}, ".eps");
  %plot_bars(aid_all(:,1:2), aid_solvers, fname, 'Solver')
endfor
%fname=strcat ("aid_precision_recall.eps");
%plot_bars(aid_together, x_labels, fname, 'Solver')


ssd_params={'0.01' '0.001' '0.0001' '0.00001' '0.000001' '0.00078125'};
ssd_solvers={'cg' 'iccg' 'bicg' 'bicgsta' 'cgs'};
for i=1:length(types)
  [ssd_all,ssd_bests_all]=data_load_detectors(WDIR,"ssd1", ssd_solvers,ssd_params,types{i});
  ssd_together = [ssd_together ; ssd_all; 0 0];
  %fname=strcat ("ssd_", types{i}, ".eps");
  %plot_bars(ssd_all(:,1:2), ssd_solvers, fname,'Solvers');
endfor
%fname=strcat ("ssd_precision_recall.eps");
%plot_bars(ssd_together, x_labels, fname, 'Solver')


newsum_params={'0.000001' '0.0000001' '0.00000001' '0.000000001' '0.0000000001'};
newsum_solvers={'cg' 'iccg' 'bicg' 'bicgsta' 'cgs'};
for i=1:length(types)
   [newsum_all,newsum_bests_all]=data_load_detectors(WDIR,"newsum", newsum_solvers,newsum_params,types{i});
   newsum_together = [newsum_together ; newsum_all; 0 0];
   %fname=strcat ("newsum_", types{i}, ".eps");
   %plot_bars(newsum_all(:,1:2), newsum_solvers, fname,'Solvers');
endfor
%fname=strcat ("newsum_precision_recall.eps");
%plot_bars(newsum_together, x_labels, fname, 'Solver')

gagan_params={'0.1'};
gagan_solvers={'cg' 'iccg' 'bicg' 'bicgsta' 'cgs'};
for i=1:length(types)
   [gagan_all,gagan_bests_all]=data_load_detectors(WDIR,"gagan", gagan_solvers,gagan_params,types{i});
   gagan_together = [gagan_together ; gagan_all; 0 0];
   %fname=strcat ("gagan_", types{i}, ".eps");
   %plot_bars(gagan_all(:,1:2), gagan_solvers, fname,'Solvers');
endfor
%fname=strcat ("gagan_precision_recall.eps");
%plot_bars(gagan_together, x_labels, fname, 'Solver')


orth_params={'0.000001' '0.0000001' '0.00000001' '0.000000001' '0.0000000001'};
orth_solvers={'cg' 'iccg' 'bicg'};
for i=1:length(types)
   [orth_all,orth_bests_all]=data_load_detectors(WDIR,"orth", orth_solvers,orth_params,types{i});
   orth_together = [orth_together ; orth_all; 0 0];
   %fname=strcat ("orth_", types{i}, ".eps");
   %plot_bars(orth_all(:,1:2), orth_solvers, fname,'Solvers');
endfor
%fname=strcat ("orth_precision_recall.eps");
%plot_bars(orth_together, x_labels, fname, 'Solver')


