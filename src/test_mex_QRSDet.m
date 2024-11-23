% example how to use mex_QRSDET

% add binary path
addpath('.. filesep bin')

fnecg= fullfile(pwd, "..","data", "ecg_int16_200Hz.dat");
fid=fopen(fnecg, 'r');
if fid < 0 
    error("could not open ecg file " + fnecg);
end    
yecg=fread(fid,'*int16');
fclose(fid);
samplefreq=200;

% generate index qrs
indx=mex_QRSDet(yecg,samplefreq);
indx_valid= indx(indx>0);
  
toc

% generate time
hr = 60./(diff(indx_valid)./samplefreq);
thr = indx_valid(2:end) ./samplefreq;
tecg = (0:length(yecg)-1) ./samplefreq;

%% plot
figure('Name','test mex_qrs')
a(1) = subplot(2,1,1);
plot(thr, hr);
ylabel('HR (bpm)')
ylim([0,150]);
a(2) = subplot(2,1,2);
plot(tecg, yecg); hold on;
plot(tecg(indx_valid),yecg(indx_valid),'*r'); 
hold off;
ylabel('ECG');
linkaxes(a,'x');

