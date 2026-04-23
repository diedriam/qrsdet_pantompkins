/*----------------------------------------------------------------------
    filter.c
	include file for filter
	version 1.0
----------------------------------------------------------------------*/

#define Rectangular		1
#define Hanning			2
#define Hamming			3
#define Blackman		4

double Pi=3.14159265358979;

#ifdef ANSI_C
float calcw(int n, int FiltWidth, int FiltType);
void calcfiltlow(float *Hlow, float fp, int FWidth, int FiltT);
void calcfiltband(float *Hband, float lowfreq, float highfreq, int FWidth, int FiltT);
#endif
#ifndef ANSI_C
float calcw();
void calcfiltlow();
void calcfiltband();
#endif


/*---------------------------------------------------------------------
	   calculating the window function for the filter
-----------------------------------------------------------------------*/
#ifdef ANSI_C
float calcw(int n, int FiltWidth, int FiltType)
#endif
#ifndef ANSI_C
float calcw(n, FiltWidth, FiltType)
int n;
int FiltWidth;
int FiltType;
#endif
{
float w = 0.;

  switch(FiltType)
  {
     case Rectangular: w = 1; 
             break;
     case Hanning    : w = .5*(1.-cos(2.*Pi*n/(FiltWidth-1)));
             break; 
     case Hamming    : w = .54-.46*cos(2.*Pi*n/(FiltWidth-1));
             break;
     case Blackman   : w = .42-.5*cos(2.*Pi*n/(FiltWidth-1))+
	       .08*cos(4.*Pi*n/(FiltWidth-1));
             break;
   }
  return w;
}



/*---------------------------------------------------------------------
	calculating the filter coefficients (low pass)

    HLow        coefficient for low pass filter
    fp          normalized passpand edge
                = cutoff frequency/sample frequency
    FWidth      width (number of poInts) of the filter
    FiltT       type of filter
	     1 Rectangular -13 dB
	 	 2 Hanning     -31 dB
		 3 Hamming     -41 dB
		 4 Blackman    -57 dB
----------------------------------------------------------------------*/
#ifdef ANSI_C
void calcfiltlow(float *Hlow, float fp, int FWidth, int FiltT)
#endif
#ifndef ANSI_C
void calcfiltlow(Hlow, fp, FWidth, FiltT)
float *Hlow;
float fp;
int FWidth;
int FiltT;
#endif
{
 int i;
     if (fp > 0)
     for(i = 0; i < FWidth; i++)
	*Hlow++ = sin(2.0*Pi*fp*(i-0.5*(FWidth-1)))
		   /(2.0*Pi*fp*(i-0.5*(FWidth-1)))
		   *calcw(i, FWidth, FiltT);
   else
      for(i = 0; i < FWidth; i++)
		  *Hlow++ = calcw(i, FWidth, FiltT);
}



/*----------------------------------------------------------------------
	   calculating the filter coefficients (band pass)

    Hband        coefficient for band pass filter
    lowfreq      normalized low freqency (LFreq/SFreq)
    highfreq     normalized high frequency (HFreq/SFreq)
----------------------------------------------------------------------*/
#ifdef ANSI_C
void calcfiltband(float *Hband, float lowfreq, float highfreq,
	int FWidth, int FiltT)
#endif
#ifndef ANSI_C
void calcfiltband(Hband, lowfreq, highfreq, FWidth, FiltT)
float *Hband;
float lowfreq;
float highfreq;
int FWidth;
int FiltT;
#endif
{
int i;
float f0, fp;

  f0 = (lowfreq+highfreq)/2;
  fp = (highfreq-lowfreq)/2; 
  calcfiltlow(Hband, fp, FWidth, FiltT);
  for(i = 0; i < FWidth; i++) {
      *(Hband) *= cos(2.0*Pi*f0*(i-0.5*(FWidth-1)));
      ++Hband;
    }

}

