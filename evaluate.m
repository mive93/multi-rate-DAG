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

maxReactionTime = 0;
maxDataAge = 0;
minLatency = 0;

if (chain=='0')
    maxReactionTime = 98.2;
    maxDataAge = 75;
    minLatency = 50.9;
elseif (chain=='1')
    maxReactionTime = 65;
    maxDataAge = 105;
    minLatency = 36.8;
elseif (chain=='2')
    maxReactionTime = 65;
    maxDataAge = 105;
    minLatency = 41.8;
elseif (chain=='3')
    maxReactionTime = 108.2;
    maxDataAge = 125;
    minLatency = 63.8;
end

figure;
subplot(2,1,1);
histogram(react, 'BinWidth', 0.5, 'FaceColor', 'blue')
xlabel('Reaction time [ms]');
ylabel('Count');
line([maxReactionTime, maxReactionTime], ylim, 'Color', 'red', 'LineWidth', 2);
xlim([minLatency, max(maxReactionTime, maxDataAge)+5]);
grid on;

subplot(2,1,2);
histogram(age, 'BinWidth', 0.5, 'FaceColor', 'blue')
xlabel('Data age [ms]');
ylabel('Count');
line([maxDataAge, maxDataAge], ylim, 'Color', 'red', 'LineWidth', 2);
xlim([minLatency, max(maxReactionTime, maxDataAge)+5]);
grid on;