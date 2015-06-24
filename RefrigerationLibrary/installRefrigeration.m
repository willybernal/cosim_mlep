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
cd(fullfile(pathstr,'Source'));
mex(fullfile(pathstr,'Source','feedinput.c'),'-v');
mex(fullfile(pathstr,'Source','mlepCosim.c'),'-v');

cd(pathstr);

disp('================================');
disp('INSTALLATION COMPLETED!!!');
disp('Install everytime Matlab opens.');
disp('================================');


