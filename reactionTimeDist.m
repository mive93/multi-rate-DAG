filename = 'bld/ages';


fid = fopen(filename);
reacts = textscan(fid,'%f%f','CollectOutput',1);
reacts = reacts{:};
fclose(fid);


scatter(reacts(:,1), reacts(:,2))