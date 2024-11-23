# qrsdet Pan-Tompkins algorithm implementation standallone and for Matlab

- Input must be 16 bit integer binary file or 16 bit matlab data.
- Best performance at 200Hz
- QRSDet is implemented as loop but be can be modified for realtime application

## How to compile mex_qrsdet
cd src
open make_mex_qrsdet

## Run test mex_qrsdet:
```
test_mex_QRSDet.m
```

## How to compile fqrsdet:
```
cd src
make fqrsdet
```

## Run test fqrsdet:
```
../bin/fqrsdet ../data/ecg_int16_200Hz.dat ../data/ecg_int16_200Hz.txt 250
```
