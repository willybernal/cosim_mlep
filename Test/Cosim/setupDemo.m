%% Disable Horizontal Scrolling
!synclient HorizTwoFingerScroll=0

%% Install Library 
p = mfilename('fullpath');
[path_str,name,ext] = fileparts(p);

installPath = fullfile(path_str,'..','..');
cd(installPath);

installRefrigeration;

cd(path_str);
%% Clear up repo 
system('rm -r eplus* *.epw in.idf mlep.log socket* sql* util* Output *.idd *.ini *grt_rtw slprj *.mexa64');

%% Open 
open('super_eplus_cases.slx');


