#ifndef QRSDET_H
#define QRSDET_H

/*----------------------------------------------------------------------
	qrsdet.h
	include file for qrs-detection
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
	Type definitions
----------------------------------------------------------------------*/
typedef struct {
	unsigned long	iNr;
	unsigned long	RRI;
	unsigned char	Test; /* 1 Byte */
} RRMsgForm;

/*----------------------------------------------------------------------
	Prototypes of Nonspecific Buffer Routines
----------------------------------------------------------------------*/
int		Sign(int value);
void	Clear(char* a, int size);
void	MoveLong(long* b, int size);
void	MoveShort(short* b, int size);
short	MedianCeil(short input, short* history, int len);

/*----------------------------------------------------------------------
	Prototypes Nonspecific Routines to input new data and update buffers
----------------------------------------------------------------------*/
void	OneStep(void);
short	NextSampleValue(void);
short	NextFiltValue(void);
short	NextDiff2Value(void);
short	NextEnergyValue(void);

/*----------------------------------------------------------------------
	Other Prototypes
----------------------------------------------------------------------*/
void	InitQRS(void);
void	CloseQRS(void);
void	QRSDet(unsigned long iStart, unsigned long iStop);
void	InitPhase1(void);
void	Phase1(unsigned long iStop);
void	Phase2(unsigned long iStop);
void	ContDetect(unsigned long iStop);
void	UpdateRRLimit(void);
int		RRLimitTest(unsigned long RR);
int		RRNormTest(unsigned long RR);
void	UpdateRRAverage(unsigned long RR);

/*----------------------------------------------------------------------
	Routines the caller (dataio.c, mex_QRSDet.c, ...) must provide
----------------------------------------------------------------------*/
short	GetSample(void);
void	PutRRRes(RRMsgForm RRRes);

/*----------------------------------------------------------------------
	Variables for QRS detection
----------------------------------------------------------------------*/
extern int T200ms;		/* 200 ms refractory period, maxHR = 300 1/min */
extern int T500ms;		/* physiological max distance 500ms, minHR = 30 1/min */

/*----------------------------------------------------------------------
	public variables
----------------------------------------------------------------------*/
extern double SampleFreq;

extern float PrLowLimit;
extern float PrHighLimit;
extern float PrMissedLimit;

/*----------------------------------------------------------------------
	private variables
----------------------------------------------------------------------*/
extern int FILTERLEN;
extern int MOVEWINDOWLEN;
extern int FILTDELAY;
extern int DERIVDELAY;
extern int INTEGRDELAY;
extern int DERIVINTEGRDELAY;
extern int TOTALDELAY;

extern int nlastin;		/* FILTERLEN 128       for next FILTER */
extern int nlastfilt;		/* TOTALDELAY 5 for next DERIVATION & SQUARING, other for graphics */
extern int nlastdiff2;		/* MOVEWINDOWLEN for next MOVING WINDOW INTEGR. */

extern short*	LastIn;
extern short*	LastFilt;
extern short*	LastDiff2;
extern float*	filtcoef;

extern unsigned long	iNrSample;
extern short	SignalValue, FiltValue, Diff2Value, EnergyValue;

extern long	LastRR[8], LastRROk[8];
extern float	RRAverage1;			/* average of most recent RR */
extern float	RRAverage2;			/* average of most recent RR that fell between limits */
extern float	RRLowLimit, RRHighLimit, RRMissedLimit;
extern int		countRRAverage1;	/* counter for RR-Intervalls not between the limits */

extern long	RRPeak;
extern long	iRRPeak, iLastOut;
extern long	lastfxIsign, fxIsign, lastfxFsign, fxFsign;
extern long	iLocMaxF, iLocMaxI, ixLocMaxF, ixLocMaxI;
extern long	distance;

extern short	LocMaxI, LocMaxF, xLocMaxF, xLocMaxI;	/* value of local maximum */

extern short	FlgI, FlgF, xFlgI, xFlgF, FlgAboveThresI, LastFlgAboveThresI;
extern int		ifound;

extern short	fxI, lastfxI, fxF, lastfxF;

extern float	ThresholdI1, ThresholdI2;	/* thresholds of window-integrated signal */
extern float	NPKI;						/* running estimate of the noise peak */
extern float	SPKI;						/* running estimate of the signal peak */
extern float	ThresholdF1, ThresholdF2;	/* thresholds of filtered signal */
extern float	NPKF;						/* running estimate of the noise peak */
extern float	SPKF;						/* running estimate of the signal peak */

#endif /* QRSDET_H */
