test = 'hercules';
chain = '0';

ageFileName = ['bld/', test, '_chain', chain, '_age'];
reactFileName =  ['bld/', test, '_chain', chain, '_react'];

fid = fopen(ageFileName);
age = textscan(fid,'%f','CollectOutput',1);
age = age{:} / 1000.0;
fclose(fid);

fid = fopen(reactFileName);
react = textscan(fid,'%f','CollectOutput',1);
react = react{:} / 1000.0;
fid = fclose(fid);


histogram(react, 'BinWidth', 0.2, 'FaceColor', 'blue')
hold on
histogram(age, 'BinWidth', 0.2, 'FaceColor', 'red')
hold off