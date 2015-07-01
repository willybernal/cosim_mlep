!synclient HorizTwoFingerScroll=0

% Install Library 
% installRefrigeration;

% Generate single individual 

t = 0:3600:1*(24)*3600;

Tlow = -30;
Thigh = -10;
Tevl = (Thigh-Tlow)*rand(25,1) + Tlow;

input.time=t;
input.signals.values=Tevl;
input.signals.dimensions=1;

fid = fopen( 'input.txt', 'a' );
for i = 1:length(Tevl)
  fprintf( fid, '%f,',Tevl(i));
end
fclose(fid);

%% Clear up repo 
system('rm -r eplus* *.epw in.idf mlep.log socket* sql* util* Output *.idd *.ini *grt_rtw slprj *.mexa64');


