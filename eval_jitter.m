test = 'hercules';
from = '7';
to = '8';

jitterFileName = ['bld/',test, '_', from, '_to_', to];

fid = fopen(jitterFileName);
jitter = textscan(fid,'%d','CollectOutput',1);
jitter = jitter{:};
jitter = jitter(10:end-1);
fclose(fid);

% histogram(jitter, 'FaceColor', 'red')

disp("0 updates: "+ sum(jitter==0)/length(jitter)*100 + "%");
disp("1 updates: "+ sum(jitter==1)/length(jitter)*100 + "%");
disp("2 updates: "+ sum(jitter==2)/length(jitter)*100 + "%");
disp("3 updates: "+ sum(jitter==3)/length(jitter)*100 + "%");
disp("4 updates: "+ sum(jitter==4)/length(jitter)*100 + "%");