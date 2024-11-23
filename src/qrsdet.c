// PRGNAME : QRSDET
// USAGE   :
// OBJECT  : REAL-TIME QRS DETECTION
//			 in human ECG for C 
// VERSION : 1.3
// LASTMOD : 16-JAN-2006
// AUTHOR  : Andre Diedrich
// HISTORY : 10-JAN-1990 Creation
// HISTORY : 23-Nov-2024 refresh code to run on Mac, Win64, Matlab2024b
// COMMENTS: works best at sample frequency 200Hz
// OPENTASK: optimize filters for different sample frequency
//
// Reference:
// J. Pan and W. J. Tompkins, "A Real-Time QRS Detection Algorithm," 
// IEEE Transactions on Biomedical Engineering, vol. BME-32, no. 3, pp. 230-236, 
// March 1985, doi: 10.1109/TBME.1985.325532. 


/*----------------------------------------------------------------------
	Nonspecific Routines for Data Manipulation
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
	function Sign
	et the next value for threshold test
----------------------------------------------------------------------*/
int	Sign(int value)
{
	return( (value<0)? -1: 1);
}


/*----------------------------------------------------------------------
	Clear
   clear at position (pointer *a) "size" bytes
----------------------------------------------------------------------*/

void Clear(char* a, int size)
{
register int i;

	for (i=0; i< size; i++)
		*(a++) = 0;
}


/*----------------------------------------------------------------------
	MoveLong
	move an long integer array
	element(n-1) -> element(n) ...
----------------------------------------------------------------------*/

void MoveLong(long* b, int size)
{
register int i;
	for (i=size-1; i>0; i--)
		*(b+i)=*(b+i-1);
}

/*----------------------------------------------------------------------
	MoveShort
	move an short integer array
	element(n-1) -> element(n) ...
----------------------------------------------------------------------*/

void	MoveShort(short* b, int size)
{
register int i;
	for (i=size-1; i>0; i--)
		*(b+i)=*(b+i-1);
}

/*--------------------------------------------------------------------
	get a new sample and update data buffer
--------------------------------------------------------------------*/
short NextSampleValue(void)
{
	MoveShort(LastIn, nlastin);
        LastIn[0] = GetSample();
	return LastIn[FILTDELAY+DERIVDELAY+INTEGRDELAY];
}
/*--------------------------------------------------------------------
    NextFiltValue
	get a filtered value and update filter history
    call NextSampleValue before call this routine to update the sample hist
    for phase correction skip FILTDELAY samples with function GetSample()
    phase shift FILTDELAY = FILTERLEN/2 where FILTERLEN is even
 --------------------------------------------------------------------*/
short NextFiltValue(void)
{
register int i;
float filtvalue = 0.;

    /* run filter */
	for (i = 0; i<FILTERLEN; i++)
		filtvalue+=filtcoef[i]*LastIn[i];
	filtvalue*=.125;


    /* move new value in filter buffer */
	MoveShort(LastFilt, nlastfilt);
    LastFilt[0] = filtvalue;
    return LastFilt[DERIVDELAY+INTEGRDELAY];
}

/*--------------------------------------------------------------------
   NextDiffValue
   get a differentiation value from filtered signal
   call NextFiltValue after calling this routine
   phase shift 2 samples!
--------------------------------------------------------------------*/
short NextDiff2Value(void)
{
short	diff2value;

	/* calculate derivation */
	diff2value = -LastFilt[4] - LastFilt[3] + LastFilt[1] + LastFilt[0];
	diff2value/= 20;
	diff2value*= diff2value;

	/* move new value in derivation buffer */
	MoveShort(LastDiff2, nlastdiff2);
    LastDiff2[0] = diff2value;
 return LastDiff2[DERIVDELAY];
}

/*--------------------------------------------------------------------
    GetEnergyValue
	do moving window integration from filtered, derivated signal
    call NextDiff2Value before calling this routine
    phase shift MOVEWINDOWLEN/2
--------------------------------------------------------------------*/
short NextEnergyValue(void)
{
int	i;
long	energyvalue = 0;

/* get new derivation value */

	/* perform moving window integration */
	for (i=0; i<MOVEWINDOWLEN; i++)
		energyvalue+= LastDiff2[i];
return  (short)(energyvalue/=MOVEWINDOWLEN);
}

/*--------------------------------------------------------------------
	   this an important routine !!!
	   it aquires new sample, handles history and stores value in SignalValue
	   it performs the filtering, stores last values and set FiltValue;
	   it performs deriv and integration, stores history and set EnergyValue
	   and moving window integration
	   and increase the running sample number index iNrSample

--------------------------------------------------------------------*/
void OneStep(void)
{
	SignalValue = NextSampleValue();   // Get phase corrected (to energy signal) next sample and update buffer
    FiltValue = NextFiltValue();       // Get phase corrected (to energy signal) next filter value and update buffer
    Diff2Value = NextDiff2Value();    //  Get phase corrected (to energy signal) next Differentiate and Square and update buffer
    EnergyValue = NextEnergyValue();  //  Get energy value = moving window integration
}
/*--------------------------------------------------------------------
       procedure UpdateRRLimit
       update the limit for RR-Interval range
--------------------------------------------------------------------*/

#ifdef ANSI_C
void UpdateRRLimit(void)
#endif
#ifndef ANSI_C
void UpdateRRLimit()
#endif
{
RRLowLimit    = PrLowLimit * RRAverage2;	// Pan Tompkins:  .92 
RRHighLimit   = PrHighLimit * RRAverage2;	// Pan Tompkins: 1.16 
RRMissedLimit = PrMissedLimit * RRAverage2; // Pan Tompkins: 1.66 
}


/*--------------------------------------------------------------------
       function RRLimitest
--------------------------------------------------------------------*/
#ifdef ANSI_C
int RRLimitTest(unsigned long RR)
#endif
#ifndef ANSI_C
int RRLimitTest(RR)
unsigned long RR;
#endif
{
return( ((RR>RRLowLimit) && (RR<RRHighLimit))? 1: 0);
}


/*--------------------------------------------------------------------
	   function RRNormlimitTest HR 30-300 1/min
--------------------------------------------------------------------*/
#ifdef ANSI_C
int RRNormTest(unsigned long RR)
#endif
#ifndef ANSI_C
int RRNormTest(RR)
unsigned long RR;
#endif
{
return( ((RR>T200ms) && (RR<T500ms))? 1: 0);
}


/*--------------------------------------------------------------------
       procedure UpdateRRAverage(RR)
       update the limit for RR-Interval range
--------------------------------------------------------------------*/
#ifdef ANSI_C
void UpdateRRAverage(unsigned long RR)
#endif
#ifndef ANSI_C
void UpdateRRAverage(RR)
unsigned long RR;
#endif
{
int i;

   if (RRLimitTest(RR)==1) {
      RRAverage2=RRAverage2-LastRROk[7]/8.0+RR/8.0;
      MoveLong(LastRROk,8); LastRROk[0]=RR;
	  countRRAverage1=0;
   }
   else countRRAverage1++;

   RRAverage1=RRAverage1-LastRR[7]/8.0+RR/8.0;
   MoveLong(LastRR,8); LastRR[0]=RR;

   if (countRRAverage1>6) {
     for (i = 0; i < 8; i++) LastRROk[i]=LastRR[i];
	 RRAverage2 = RRAverage1;
     countRRAverage1=0;

   }
}

/*--------------------------------------------------------------------

	initialization of QRS-filter and other coefficients for QRSDet

--------------------------------------------------------------------*/
#ifdef ANSI_C
void InitQRS(void)
#endif
#ifndef ANSI_C
void InitQRS()
#endif
{

	/* for buffer system */
	FILTERLEN =  (int) (0.5*SampleFreq); //filter length must be odd;
	if (FILTERLEN % 2 != 0) FILTERLEN++;
	if (FILTERLEN < 32) FILTERLEN=32;
	MOVEWINDOWLEN = (int) (0.15*SampleFreq);
	if (MOVEWINDOWLEN % 2 != 0) MOVEWINDOWLEN++;
	/* delays for calculation of buffer depth */
	FILTDELAY = (int) (0.5*FILTERLEN);
	DERIVDELAY =2;
	INTEGRDELAY =  (int)(0.5*MOVEWINDOWLEN);
	DERIVINTEGRDELAY = DERIVDELAY+INTEGRDELAY;
	TOTALDELAY = FILTDELAY+DERIVDELAY+INTEGRDELAY;



	/* for buffer depth */
    /* DERIVELEN = 5 */
	nlastin	  = FILTERLEN+5+MOVEWINDOWLEN; /* sample input for FILTER and align total delay*/
	nlastfilt = FILTERLEN+5+MOVEWINDOWLEN; /* filtered input for DERIVATION & SQUARING */
	nlastdiff2 =            MOVEWINDOWLEN; /* diff2 input for MOVING WINDOW INTEGR. */

	/* create buffers */
	LastIn = (short*) calloc(nlastin,sizeof(short));
	LastFilt = (short*) calloc (nlastfilt,sizeof(short));
	LastDiff2 = (short *) calloc(nlastdiff2,sizeof(short));


	/* init buffers */
	Clear((char*)LastRR,sizeof(LastRR));
	Clear((char*)LastRROk,sizeof(LastRROk));




	/* create filter */
	filtcoef = (float *) calloc(FILTERLEN,sizeof(float));

	calcfiltband(filtcoef, 5.0/SampleFreq, 12.0/SampleFreq, FILTERLEN, Blackman);

	/* for criteria */
	T200ms = (int) (0.2 * SampleFreq);  // Refractory time
	T500ms = (int) (0.5 * SampleFreq);	// maximal RR for 30 bpm



	// limits for regular and missing beat
	// Pan Tompkins:
    // PrLowLimit  .92 
	// RRHighLimit   1.16 
	// RRMissedLimit 1.66 
	PrLowLimit=0.5;
    PrHighLimit=1.33;
	PrMissedLimit=1.66;



}

/*--------------------------------------------------------------------

	close: deallocate all dynamic buffers

--------------------------------------------------------------------*/
#ifdef ANSI_C
void CloseQRS(void)
#endif
#ifndef ANSI_C
void CloseQRS()
#endif
{
		if (LastIn != NULL)  free(LastIn);
        LastIn=NULL;
		if (LastFilt != NULL) free(LastFilt);
        LastFilt=NULL;
		if (LastDiff2 != NULL) free(LastDiff2);
        LastDiff2=NULL;
		if (filtcoef != NULL) free(filtcoef);
        filtcoef=NULL;
}


/*--------------------------------------------------------------------
	initialiazation of RR-Analyse
--------------------------------------------------------------------*/
#ifdef ANSI_C
void InitPhase1(void)
#endif
#ifndef ANSI_C
void InitPhase1()
#endif
{
	fxIsign = fxFsign = -1; fxI = fxF = 0;
	NPKI = NPKF =  99999.0; SPKI = SPKF = -99999.0;

	countRRAverage1 = RRLowLimit = RRHighLimit = RRMissedLimit = 0;

	lastfxI = fxI = fxF = lastfxF = RRPeak = iRRPeak = iLastOut	= 0;
	lastfxIsign = fxIsign = lastfxFsign = fxFsign = 0;

   	ThresholdI1 = ThresholdI2 = NPKI = SPKI = ThresholdF1	= 0.0;
   	ThresholdF2 = NPKF = SPKF = RRAverage1 = RRAverage2 = 0.0;

   	LocMaxI = LocMaxF = xLocMaxF = xLocMaxI = 0;
	iLocMaxF = iLocMaxI = ixLocMaxF = ixLocMaxI = 0L;

   	FlgI = FlgF = xFlgI = xFlgF = FlgAboveThresI = LastFlgAboveThresI = 0;

   	ifound = 0; distance = 0L;

	Clear((char*)LastRR,sizeof(LastRR));
	Clear((char*)LastRROk,sizeof(LastRROk));


}

short   MedianCeil(short input, short* history, int len)
{
    //nmed must be odd and greater than 2
    int i=0;
    int j;
    int imed=len/2+1;
    if ( input > history[0] ){
        // is input smaller than shift from start
        while((input>history[i]) && (i++ < len-2));
        for (j=1; j<=i; j++) history[j-1]=history[j];
        history[i]=input;
    }
     return history[len/2];
}

/*--------------------------------------------------------------------
	Learnphase 1: Initialize thresholds
--------------------------------------------------------------------*/
#ifdef ANSI_C
void Phase1(unsigned long iStop)
#endif
#ifndef ANSI_C
void Phase1(iStop)
unsigned long iStop;
#endif
{
    #define nmed 5
    short npkimin[nmed], npkimax[nmed];
    short npkfmin[nmed], npkfmax[nmed];
	register int i, ii;
    short data[7]={1,2,3,14,15,16,20};
    short help;

    for (ii=0; ii<nmed;ii++) {
        npkimin[ii]=9999.0;
        npkimax[ii]=-9999.0;
        npkfmin[ii]=9999.0;
        npkfmax[ii]=-9999.0;
    }

	while(((SPKI < 2*NPKI) || (SPKI == -99999.0)  ||
		(NPKI == 99999.0)  || (SPKF == -99999.0)  ||
		(NPKF == 99999.0)  || (ThresholdI1 == 0)) &&
		(iNrSample < iStop))
   {

     /* init 4 sec */
	  for(i = 0; i < 3. * SampleFreq; ++i)
	  {
	    OneStep();

	    // store last filter values
        lastfxF     = fxF;
        fxF         = FiltValue;
        lastfxFsign = fxFsign;

        // update sign for float
		fxFsign= Sign(fxF-lastfxF);

		  // check for local maxima
		  if (lastfxFsign > 0 && fxFsign < 0)
		  {

			   // store minimal value to noise level
        	   if (NPKF>fxF) NPKF=fxF;

        	   // store maximal value to signal level
        	   //if (SPKF<fxF)
                   SPKF=fxF = MedianCeil(fxF,  npkfmax, nmed);

           }

		// store last energy value
        lastfxI     = fxI;
        fxI         = EnergyValue;
        lastfxIsign = fxIsign;
		fxIsign=Sign(fxI - lastfxI);

		  // check for local maxima	x
		  if (lastfxIsign > 0 && fxIsign < 0)
		  {
    	   // store minimal value to noise level
           if (NPKI > fxI) NPKI = fxI;
       	   // store maximal value to signal level
           //if (SPKI < fxI)
               SPKI = fxI = MedianCeil(fxI,  npkimax, nmed);
          }

     } /* end of for */
	ThresholdI1 = NPKI + 0.25 * (SPKI - NPKI);
	ThresholdI2 = 0.5 * ThresholdI1;
	ThresholdF1 = NPKF + 0.25 * (SPKF - NPKF);
	 ThresholdF2 = 0.5 * ThresholdF1;
	}/* end of while */


} /* end of phase1 */



/*--------------------------------------------------------------------
	   this routine waits until two sucsesive R-Peaks have been
           detected.
--------------------------------------------------------------------*/
#ifdef ANSI_C
void Phase2(unsigned long iStop)
#endif
#ifndef ANSI_C
void Phase2(iStop)
unsigned long iStop;
#endif
{
RRMsgForm	RRRes;

	while((ifound < 2) && (iNrSample < iStop))
   {
		OneStep();
		// store and update filtered value
		lastfxF = fxF;
		fxF     = FiltValue;

		// store and update last direction of filtered signal
		lastfxFsign = fxFsign;
		fxFsign     = Sign(fxF-lastfxF);

		// store and update energy value
		lastfxI = fxI;
		fxI     = EnergyValue;

		// store and update last direction of energy signal
		lastfxIsign = fxIsign;
		fxIsign     = Sign(fxI-lastfxI);


		// store and update flag above threshold
		LastFlgAboveThresI = FlgAboveThresI;
		FlgAboveThresI     = Sign(fxI-ThresholdI1);

		// local maxima in filtered signal?
		if ( lastfxFsign > 0 && fxFsign < 0)
		{
			// local maximal bigger then previous and energy above threshold?
			if (fxF > xLocMaxF && FlgAboveThresI > 0)
			{
				if (xFlgF > 1)
			  	{
				  // accept and asign the local Max
				  LocMaxF = xLocMaxF;
				  // set FlgF zero to indicate loc maxima detected
				  FlgF=0;
				}
			    // store also the values in temp loc max
			    xLocMaxF  = fxF;
			    ixLocMaxF = iNrSample;
			    // set Flg one to indicate loc maxima above threshold I detected
			    xFlgF     = 1;
	 		} else {

			  	LocMaxF = fxF;
			  // set FlgF zero to indicate loc maxima detected
			  	FlgF    = 0;
	 		}
		}
		// set Flg to -1 to indicate that current sample is not a loc maxima
		else FlgF = -1;

		//  local maximum in energy function ?
		if (lastfxIsign > 0 && fxIsign< 0)
		{
			// if local maxima is greater than current and above threshold and distance bigger than
			// refractory period
			if ((fxI > xLocMaxI) && (FlgAboveThresI > 0) && ((iNrSample-iRRPeak+1)> 0.2*SampleFreq))
			{
				if (xFlgI > 0) {
					LocMaxI = xLocMaxI; FlgI = 0;
	    		}
				xLocMaxI = fxI; ixLocMaxI = iNrSample; xFlgI = 1;
	 		} else {
	 	   		LocMaxI = fxI; FlgI = 0;
	 		}
       	} else FlgI=-1;

		if (LastFlgAboveThresI > 0 && FlgAboveThresI < 0)
		{

			if (xFlgI > 0  && xFlgF>0)
			{
				SPKI = 0.125 * LocMaxI + 0.875 * SPKI;
				ThresholdI1 = NPKI + 0.25 * (SPKI-NPKI);
				ThresholdI2 = 0.5 * ThresholdI1;

				SPKF= 0.125 * LocMaxF + 0.875 * SPKF;
		    	ThresholdF1 = NPKF + 0.25 * (SPKF-NPKF);
				ThresholdF2 = 0.5 * ThresholdF1;

				ifound++;
				distance    = ixLocMaxF - iRRPeak;
				iRRPeak     = ixLocMaxF;
				RRPeak      = xLocMaxF;
				xLocMaxF    = xLocMaxI= -999.0;
				xFlgF       = xFlgI = -1;

				RRRes .RRI= distance;
				RRRes.Test=0;
				RRRes.iNr=ixLocMaxF-TOTALDELAY-1;

				// output of the result
				PutRRRes(RRRes);
#ifdef DEBUG
				 mexPrintf("found* %ld\n", RRRes.iNr);
#endif
			} else {
				 LocMaxF  = xLocMaxF;
				 LocMaxI  = xLocMaxI;
				 FlgF     = xFlgF;
				 FlgI     = xFlgI;
				 xLocMaxF = xLocMaxI= -999.0;
				 xFlgF    = xFlgI= -1;
			}
      	}
		if (FlgF>-1)
		{
	 		NPKF = 0.125 * LocMaxF + 0.875 * NPKF;
	 		ThresholdF1 = NPKF + 0.25 * (SPKF - NPKF);
	 		ThresholdF2 = 0.5 * ThresholdF1;
	 		FlgF = -1;
      	}
		if (FlgI > -1)
		{
			NPKI=0.125*LocMaxI+0.875*NPKI;
			ThresholdI1=NPKI+0.25*(SPKI-NPKI);
	 		ThresholdI2=0.5*ThresholdI1;
	 		FlgI=-1;
      	}
   } //end of while(iFound<2)  &  while RRNormTest
} // end of Phase2()


/*--------------------------------------------------------------------
					 continous detection
--------------------------------------------------------------------*/
#ifdef ANSI_C
void ContDetect(unsigned long iStop)
#endif
#ifndef ANSI_C
void ContDetect(iStop)
unsigned long iStop;
#endif
{
RRMsgForm	RRRes;
 iLastOut=iNrSample;

	while(iNrSample < iStop)
	{
      OneStep();
      	// store last filt value and update new filt value
		lastfxF = fxF;
		fxF     = FiltValue;

      	// store last sign of change and update new sign of change
		lastfxFsign = fxFsign;
		fxFsign     = Sign(fxF-lastfxF);

      	// store last energy value and update new energy value
		lastfxI = fxI;
		fxI     = EnergyValue;

      	// store last sign of change and update new sign of change
		lastfxIsign = fxIsign;
		fxIsign     = Sign(fxI-lastfxI);

      	// store last Flag above Threshold
		LastFlgAboveThresI = FlgAboveThresI;
		FlgAboveThresI     =((fxI > ThresholdI2)? 1:-1);

		// local maxima in filt signal?
		if (lastfxFsign > 0 && fxFsign < 0)
		{
			// maxima greater then prev and above threshold?
			if (fxF > xLocMaxF && FlgAboveThresI > 0)
			{
				// set flag LocMax is
				if (xFlgF > 1)
				{
					// accept the local max
					LocMaxF = xLocMaxF;
	       			FlgF    = 0;
	    		}

	    		// store value in tmp loc maxima
	    		xLocMaxF  = fxF;
	    		ixLocMaxF = iNrSample;
	    		// set flag tmp loc maxima occurred
	    		xFlgF     = 0;
	 		}
			else
			{
			// store local axima
	    	LocMaxF = fxF;
	 	   // set Flag: "local maxima below Thres"
	    	FlgF    = 0;
	 		}
		}
		// set Flag "no local maxima"
		else FlgF = - 1;


		// is local maxima in energy signal?
		if (lastfxIsign > 0 && fxIsign < 0)
		{
			if (fxI > xLocMaxI && FlgAboveThresI > 0)
			{
				if (xFlgI>1)
				{
	    	   		LocMaxI = xLocMaxI;
	    	   		FlgI    = 0;
	    		}
	    		xLocMaxI = fxI;
	    		iLocMaxI = iNrSample;
	    		xFlgI    = 0;
	 	}
		else
		{
	 	   LocMaxI = fxI;
	 	   // set Flag: "local maxima below Thres"
	 	   FlgI    = 0;
	 	}
		// set Flag "no local maxima"
      } else FlgI = -1;

		if (LastFlgAboveThresI > 0 && FlgAboveThresI < 0)
		{
	 if (xLocMaxF > ThresholdF2) xFlgF = 2;
	 if (xLocMaxF > ThresholdF1) xFlgF = 1;
	 if (xLocMaxI > ThresholdI2) xFlgI = 2;
	 if (xLocMaxI > ThresholdI1) xFlgI = 1;

	 distance = ixLocMaxF-iRRPeak+1;

			if ((xFlgI>0 && xFlgF>0) &&  ((iNrSample-iRRPeak+1)> T200ms ) && (distance>RRLowLimit))
			{

					if (xFlgI == 1)
					   SPKI = 0.125 * LocMaxI + 0.875 * SPKI;
					else
					   SPKI = 0.25 * LocMaxI + 0.75 * SPKI;
	       ThresholdI1 = NPKI + 0.25 * (SPKI - NPKI);
	       ThresholdI2 = 0.5 * ThresholdI1;

					if (xFlgF==1)
					   SPKF = 0.125 * LocMaxF + 0.875 * SPKF;
					else
					   SPKF = 0.25 * LocMaxF + 0.75 * SPKF;
	       ThresholdF1 = NPKF + 0.25 * (SPKF - NPKF);
					ThresholdF2 = 0.5 * ThresholdF1;


	       RRRes.RRI= distance;
	       RRRes.Test=RRLimitTest(distance);
		   RRRes.iNr=ixLocMaxF-TOTALDELAY-1;


		   /* user defined output of the result */
		   PutRRRes(RRRes);

	       iLastOut = ixLocMaxF;
	       UpdateRRAverage(distance);
	       UpdateRRLimit();
	       iRRPeak = ixLocMaxF;
	       RRPeak  = xLocMaxF;
	       xLocMaxF = xLocMaxI=-999.0;
	       xFlgF = xFlgI = -1;
	    }
			else
			{
	       LocMaxF  = xLocMaxF;
	       LocMaxI  = xLocMaxI;
			   FlgF     = xFlgF;
	       FlgI     = xFlgI;
	       xLocMaxF = xLocMaxI =-999.0;
	       xFlgF    = xFlgI = -1;
	 }
      }
	  if (FlgF>-1)
	  {
	 NPKF        = 0.125 * LocMaxF + 0.875 * NPKF;
	 ThresholdF1 = NPKF + 0.25 * (SPKF - NPKF);
	 ThresholdF2 = 0.5 * ThresholdF1;
	 FlgF        = -1;
      }
		if (FlgI>-1)
		{
	 NPKI        = 0.125 * LocMaxI + 0.875 * NPKI;
	 ThresholdI1 = NPKI + 0.25 * (SPKI - NPKI);
	 ThresholdI2 = 0.5 * ThresholdI1;
	 FlgI        = -1;
      }

	} /* end of while(1)  */

} /* end of ContDetect */



/*--------------------------------------------------------------------

	QRSDET Main Routine
	Please Run QRSInit before using QRSDET
	This routine require user defined routines: getsample and putrr

--------------------------------------------------------------------*/

#ifdef ANSI_C
void	QRSDet(unsigned long iStart, unsigned long iStop)
#endif
#ifndef ANSI_C
void	QRSDet(iStart, iStop)
unsigned long iStart;
unsigned long iStop;
#endif
{
unsigned long	i;

   {
	// fill the buffers of filter and energy function

    iNrSample = iStart;
	for (i = 0; i<= TOTALDELAY; i++) OneStep();
   	InitPhase1();


     /* Learnphase 1 */
    // Learnphase 1
	Phase1(iStop);

	// Learnphase 2
	ifound  = 0;
	iRRPeak = iNrSample;
#undef DEBUG
#ifdef DEBUG
   	mexPrintf("ThresI1 %g\n", ThresholdI1);
    mexPrintf("ThresI2 %g\n", ThresholdI2);
   	mexPrintf("ThresF1 %g\n", ThresholdF1);
    mexPrintf("ThresF2 %g\n", ThresholdF2);
    mexPrintf("NPKI %g\n", NPKI);
    mexPrintf("NPKF %g\n", NPKF);
    mexPrintf("SPKI %g\n", SPKI);
    mexPrintf("SPKF %g\n", SPKF);
#endif
	Phase2(iStop);

	RRAverage1= RRAverage2 = distance;

#ifdef DEBUG
	mexPrintf("distance =%ld", distance);
#endif

	for (i=0; i<8; i++)  LastRR[i]=LastRROk[i] = distance;
	countRRAverage1 = 0;
	UpdateRRLimit();

    // Detectionphase /
	ContDetect(iStop);
   }
}



