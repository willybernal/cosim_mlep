%% This script install the Simulink Library and its dependencies

% Get current path
p = mfilename('fullpath');
[pathstr,name,ext] = fileparts(p);

% Add paths
% Library
addpath(fullfile(pathstr,'RefrigerationLibrary'));
% Source Code
addpath(fullfile(pathstr,'RefrigerationLibrary','Source'));
% Images
addpath(fullfile(pathstr,'RefrigerationLibrary','Image'));
% bcvtb
addpath(genpath(fullfile(pathstr,'RefrigerationLibrary','bcvtb')));

% Generate Mex 
cd(fullfile(pathstr,'RefrigerationLibrary','Source'));
mex(fullfile(pathstr,'RefrigerationLibrary','Source','feedinput.c'),'-v');
mex(fullfile(pathstr,'RefrigerationLibrary','Source','mlepCosim.c'),'-v');
mex(fullfile(pathstr,'RefrigerationLibrary','Source','writeoutput.c'),'-v');
mex(fullfile(pathstr,'RefrigerationLibrary','Source','writeresult.c'),'-v');
cd(pathstr);

disp('                                ');
disp('================================');
disp('      REFRIGERATION PROJECT     ');
disp('    INSTALLATION COMPLETED!!!   ');
disp('Install everytime Matlab opens. ');
disp('================================');


