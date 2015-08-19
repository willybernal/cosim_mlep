!synclient HorizTwoFingerScroll=0

% Install Library 
% installRefrigeration;

% Generate single individual 

t = 0:3600:1*(24)*3600;

Tlow = -30;
Thigh = -10;
Tevl = (Thigh-Tlow)*rand(25,1) + Tlow;
Tevl = 1:1:24;

input.time=t;
input.signals.values=Tevl;
input.signals.dimensions=1;

constant = [12,15,3*10^5];

% fid = fopen( 'input.txt', 'w+');
csvwrite('input.txt',Tevl);
csvwrite('constant.txt',constant);
% fclose(fid);

%% Clear up repo 
system('rm -r eplus* *.epw in.idf *.log socket* sql* util* Output *.idd *.ini *grt_rtw slprj *.mexa64 *~');
clc;
%% Open simulink
open('retail_store.slx');
clc;

