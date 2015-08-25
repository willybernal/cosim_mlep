% Validate 
filename = 'validation-walkin.csv';

% Open TMY3 data
fid = fopen(filename,'r');
assert(fid >= 0, ...
    'validation:fileNotFound', ...
    ['data file ''%s'' ' ...
    'is not present in the current MATLAB path.'], ...
    filename);

temp = textscan(fid, '%s %f %f %f %f %f','Delimiter',',','EmptyValue', NaN,'TreatAsEmpty','(null)','HeaderLines', 2);
fclose(fid);

tevl = (temp{3}-32)*5/9;
tprod = (temp{4}-32)*5/9;
tamb = (temp{5}-32)*5/9;
tout = (temp{6}-32)*5/9;

% tevl = temp{3};
% tprod = temp{4};
% tamb = temp{5};
% tout = temp{6};

walkin.time = [0:5*60:(length(tevl)-1)*5*60]';
walkin.signals.values = [tevl, tprod, tamb, tout];
walkin.signals.dimensions = 4;

%%
filename = 'validation-reachin.csv';

% Open TMY3 data
fid = fopen(filename,'r');
assert(fid >= 0, ...
    'validation:fileNotFound', ...
    ['data file ''%s'' ' ...
    'is not present in the current MATLAB path.'], ...
    filename);

temp = textscan(fid, '%s %f %f %f %f %f','Delimiter',',','EmptyValue', NaN,'TreatAsEmpty','(null)','HeaderLines', 2);
fclose(fid);

tevl = (temp{3}-32)*5/9;
tprod = (temp{4}-32)*5/9;
tamb = (temp{5}-32)*5/9;
tout = (temp{6}-32)*5/9;

% tevl = temp{3};
% tprod = temp{4};
% tamb = temp{5};
% tout = temp{6};

reachin.time = [0:5*60:(length(tevl)-1)*5*60]';
reachin.signals.values = [tevl, tprod, tamb, tout];
reachin.signals.dimensions = 4;

save('validation-data.mat','reachin','walkin');
