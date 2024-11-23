# qrsdet Pan-Tompkins algorithm implementation standalone and for Matlab

- old implementation but still runs
- input must be 16 bit integer binary file or 16 bit matlab data.
- best performance at sample freq 200Hz
- runs on Linux, Mac, Win, and Matlab
- QRSDet is implemented as loop for file processing but it be can be modified for realtime application
- dataio.c contains routines for get and put data 

## How to compile mex_qrsdet for Matlab
cd src
open make_mex_qrsdet

## Run test mex_qrsdet in Matlab:
```
test_mex_QRSDet.m
```
## How to compile fqrsdet as for file processing
```
cd src
make fqrsdet
```

## Run test fqrsdet for fileprocessing:
```
../bin/fqrsdet ../data/ecg_int16_200Hz.dat ../data/ecg_int16_200Hz.txt 250
```
