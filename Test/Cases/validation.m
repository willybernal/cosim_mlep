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

walkin.tevl = temp{3};
walkin.tprod = temp{4};
walkin.tamb = temp{5};
walkin.tout = temp{6};

walkin.time = 0:5*60:length(walkin.tevl)*5*60;
walkin.signals.values = 0;
walkin.signals.dimensions = [4];

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

reachin.tevl = temp{3};
reachin.tprod = temp{4};
reachin.tamb = temp{5};
reachin.tout = temp{6};
