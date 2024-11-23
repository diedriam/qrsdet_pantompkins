
/*----------------------------------------------------------------------
	qrsdet.h
	include file for qrs-detection
	version 3.0 (Visual C++ 6.0 for Windows)
----------------------------------------------------------------------*/


/*----------------------------------------------------------------------
	Prototypes of Nonspecific Buffer Routines
----------------------------------------------------------------------*/
int	Sign(int value);
void	Clear(char* a, int size);
void	MoveLong(long* b, int size);
void	MoveShort(short* b, int size);
short   MedianCeil(short input, short* history, int len);
/*----------------------------------------------------------------------
	Prototypes Nonspecific Routines to input new data and update buffers
----------------------------------------------------------------------*/
void	OneStep(void);
short   NextSampleValue(void);
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
	int	RRLimitTest(unsigned long RR);
	int	RRNormTest(unsigned long RR);
	void	UpdateRRAverage(unsigned long RR);


/*----------------------------------------------------------------------
	Type definitions
----------------------------------------------------------------------*/
typedef struct{
   unsigned long	iNr;
   unsigned long	RRI;
   unsigned char	Test; /* 1 Byte */
}RRMsgForm;

/*----------------------------------------------------------------------
	Variables for QRS detection
----------------------------------------------------------------------*/

	int T200ms;				//#define	T200ms				51   200 ms refractory period  maxHR = 300 1/min
	int T500ms;			//#define T500ms				128	 physiological max distance 500ms minHR = 30 1/min
/*----------------------------------------------------------------------
	public variables
----------------------------------------------------------------------*/

	double SampleFreq;
/*--------------------------------------------------------------------*/
	float	PrLowLimit;
	float	PrHighLimit;
	float	PrMissedLimit;
/*----------------------------------------------------------------------
	private variables
----------------------------------------------------------------------*/


	int FILTERLEN;
	int MOVEWINDOWLEN;
	int FILTDELAY;
	int DERIVDELAY;
	int INTEGRDELAY;
	int DERIVINTEGRDELAY;
	int TOTALDELAY;

	int nlastin;	// FILTERLEN 128		 for next FILTER 
	int nlastfilt;	//TOTALDELAY 5 for next DERIVATION & SQUARING other for graphics 
	int nlastdiff2;	//MOVEWINDOWLEN	/* for next MOVING WINDOW INTEGR. 

	short*	LastIn = NULL;
	short*	LastFilt =NULL;
	short* LastDiff2 = NULL;
	float* filtcoef = NULL;



	unsigned long	iNrSample = 0L;
	short	SignalValue, FiltValue, Diff2Value, EnergyValue;

	long	LastRR[8], LastRROk[8];
	float	RRAverage1;                 /* average of most recent RR  */
	float	RRAverage2;                 /* average of most recent RR
                                        that fell between limits      */
	float	RRLowLimit, RRHighLimit, RRMissedLimit;
	int	countRRAverage1;            /* counter for RR-Intervalls,
                                        not between the limits        */

	long	RRPeak;
	long	iRRPeak, iLastOut;
	long	lastfxIsign, fxIsign, lastfxFsign, fxFsign;
	long	iLocMaxF, iLocMaxI, ixLocMaxF, ixLocMaxI;
	long	distance;

	short	LocMaxI, LocMaxF,
			xLocMaxF, xLocMaxI;         /* value of local maximum    */

	short	FlgI, FlgF, xFlgI, xFlgF, FlgAboveThresI, LastFlgAboveThresI;
	int  	ifound;

	short	fxI, lastfxI, fxF, lastfxF;

	float	ThresholdI1, ThresholdI2;   /* thresholds of window-
												Integrated signal     */
	float	NPKI;                       /* running estimate of the
                                               noise  peak            */
	float	SPKI;                       /* running estimate of the
                                               signal peak            */
	float	ThresholdF1, ThresholdF2;   /* thresholds of filtered
                                               signal                 */
	float	NPKF;                       /* running estimate of the
											   noise  peak            */
	float	SPKF;                       /* running estimate of the
                                               signal peak            */

/*--------------------------------------------------------------------*/