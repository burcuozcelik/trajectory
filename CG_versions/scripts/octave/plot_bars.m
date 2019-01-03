function plot_bars(data, xnames, fname, ixlabel)
  figure;
  bar(data);
  ldesc={'Precision', 'Recall'};

  ylabel('Percentage (%)','fontweight','bold','fontsize',14);
  xlabel(ixlabel,'fontweight','bold','fontsize',14);
  ylim([0 1]);

  h=get(gcf,'currentaxes');
  set (gca, 'xtick', 1:size(xnames,2));
  set (gca, 'xticklabel', xnames);
  set (gca, 'ytick', [0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1]);
  set (gca, 'yticklabel', [0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1]);
  set(h,'fontweight','bold','linewidth',2,'fontsize',14);
  legend(ldesc,'orientation','horizontal','location','north');
  grid on;

  
  %fname=strcat (fname, "_precision_recall.eps");
  print(fname, '-depsc2', '-S600,400');

end
