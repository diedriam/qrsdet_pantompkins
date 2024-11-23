% PRGNAME : make mex_QRSDet
% OBJECT  : make Interface to read windaq format into MATLAB
% USAGE   : [data,fs,nsamples,labels, units, slope, intercept,indx_marker,markertext,strMarkerTime, Timestamps]=mex_QRSDet(filename);
% VERSION : 1
% LASTMOD : 17-June-2012 
% AUTHOR  : Andre Diedrich
% HISTORY : 27-Dec-2012
%
%% compile

if ispc
    display('compiling mex_QRSDet.c...');
    mex mex_QRSDet.c -output ../bin/mex_QRSDet
else
    display('compiling mex_QRSDet.c...');
    mex mex_QRSDet.c -output ../bin/mex_QRSDet
end 

display('compilation done.');
display('usage:');
display('indx = mex_qrsdet(ecg, samplefreq)');
display('test:');
display('run "test_mex_QRSDet"');

