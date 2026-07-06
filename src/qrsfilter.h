#ifndef QRSFILTER_H
#define QRSFILTER_H

/*----------------------------------------------------------------------
	qrsfilter.h
	include file for filter
----------------------------------------------------------------------*/

#define Rectangular		1
#define Hanning			2
#define Hamming			3
#define Blackman		4

extern double Pi;

float	calcw(int n, int FiltWidth, int FiltType);
void	calcfiltlow(float *Hlow, float fp, int FWidth, int FiltT);
void	calcfiltband(float *Hband, float lowfreq, float highfreq, int FWidth, int FiltT);

#endif /* QRSFILTER_H */
