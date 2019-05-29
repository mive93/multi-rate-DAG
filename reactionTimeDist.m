filename = 'bld/ages';


fid = fopen(filename);
reacts = textscan(fid,'%f%f%f','CollectOutput',1);
reacts = reacts{:};
fclose(fid);


scatter3(reacts(:,1), reacts(:,2), reacts(:,3))