%% This script install the Simulink Library and its dependencies

% Get current path
p = mfilename('fullpath');
[pathstr,name,ext] = fileparts(p);

% Add paths
% Library
addpath(pathstr);
% Source Code
addpath(fullfile(pathstr,'Source'));
% Images
addpath(fullfile(pathstr,'Image'));

% Generate Mex 
mex(fullfile(pathstr,'Source','feedinput.c'));
mex(fullfile(pathstr,'Source','mlepCosim.c'));

disp('================================');
disp('INSTALLATION COMPLETED!!!');
disp('Install everytime Matlab opens.');
disp('================================');


