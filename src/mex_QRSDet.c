// PRGNAME : mex_QRSDET
// USAGE   : [indx_rr] = mex_QRSDet(yin, SampleFreq)
// OBJECT  : QRS DETECTION in human ECG for MATLAB (modified Pan Tompkins)
// VERSION : 1.3
// LASTMOD : 17-JAN-2006
// AUTHOR  : Andre Diedrich
// HISTORY : 10-JAN-2005 Creation
// OPENTASK: INPUT PARM FOR FILTER, LIMITS, SMALL ANIMAL MODE
// DEBUG   : [indx_rr, yout, fy, energy] = mex_QRSDet(yin, SampleFreq)


#include "mex.h"
#include "math.h"

#include "stdio.h"

#define ANSI_C

#ifndef ANSI_C
	#define SEEK_SET 0
	#define SEEK_CUR 1
	#define SEEK_END 2
#endif


#define DEBUG 1
//#undef DEBUG

#include "qrsdet.h"

#include "qrsfilter.c"

/*--------------------------------------------------------------------
    GetSample
	Get a new sample from input file
--------------------------------------------------------------------*/

short *ptr_yin;
double *ptr_yqrsout, *ptr_y, *ptr_fy, *ptr_dy, *ptrdbl;

double ymin, ymax, yscale;

long indx,epsilon, ii;	//for search maxima in raw signal;


/* for reallocation */
int new_dims[2];


/*--------------------------------------------------------------------
    GetSample
	Get a new sample from input file
--------------------------------------------------------------------*/

double ntotal;
long waitbar_step;
mxArray* mxParam[2];
mxArray* mxwaitbar_handle;
mxArray* mxwaitbar_index;
mxArray* mxwaitbar_string;
double* prwaitbar_index;

// this routine assigned new sample and increased index
short GetSample(void)
{
   if (iNrSample % waitbar_step ==0){
		(*prwaitbar_index)= (double)iNrSample/ntotal;
		mexCallMATLAB(1,&mxwaitbar_handle,1, &mxwaitbar_index, "waitbar");
    }

	//return (short) (((*(ptr_yin+iNrSample++))-ymin)*yscale );
	return (short) (*(ptr_yin+iNrSample++));
}


/*----------------------------------------------------------------------
	Specific Routine for Output of RRResult;
----------------------------------------------------------------------*/
void	PutRRRes(RRMsgForm RRRes)
{
	// binary
	//	fwrite(&RRRes, sizeof(RRRes), 1, RRFile);

	/* output in ASCII Time format in ms
	fprintf(RRFile,"%ld\t%ld\t%d\n",
	   (RRRes.iNr*1000L)/(long)SampleFreq,
	   (RRRes.RRI*1000L)/(long)SampleFreq,
	   RRRes.Test);*/

	/* output in ASCII format (sampleunits)
       fprintf(RRFile,"%ld\t%ld\t%d\n",
	   RRRes.iNr, RRRes.RRI, RRRes.Test); */

	/* find next maxima in the unfiltered ecg */

	double ymax= *(ptr_yin+RRRes.iNr);
	unsigned long imax, i;
    // Matlab indx counts from 1
	imax = RRRes.iNr+1;

    // search for max in neighborhood in raw signal
	for (i=RRRes.iNr-epsilon-1; i<RRRes.iNr+epsilon; i++) {
		if (ymax < *(ptr_yin+i)){
			ymax= *(ptr_yin+i);
			imax=i;
		}
	}

	*(ptr_yqrsout+indx++) = imax+1;
}

/*--------------------------------------------------------------------*/

#include "qrsdet.c"

/*--------------------------------------------------------------------

	Main Matlab Mex Entry

--------------------------------------------------------------------*/


void mexFunction(
                 int nlhs,       mxArray *plhs[],
                 int nrhs, const mxArray *prhs[]
		 ){
  char errMsg[100];
  if(!mxIsInt16(prhs[0])) {
       const char str[]="Function requires int16 input for wave data";

       sprintf(errMsg,"%s '%s'\n",str,mxGetClassName(prhs[0]));
       mexErrMsgTxt(errMsg);
  } else {
     /* causes MATLAB to execute the string as an expression
     * or statement the biggest limitation is that it won't
     * evaluate any left-hand arguments except 'ans'
     */



     unsigned long mrows, ncols;
     mrows = mxGetM(prhs[1]);
     ncols = mxGetN(prhs[1]);

     /* The samplefreq must be a noncomplex scalar double.*/
     if (!mxIsDouble(prhs[1]) || mxIsComplex(prhs[0]) ||
         !(mrows == 1 && ncols == 1))
     {
         mexErrMsgTxt("usage: [indx_rr] = mex_QRSDet(yin, SampleFreq)\n");
     }

     // SampleFreq is already defined in qrsdet.h
     SampleFreq = mxGetScalar(prhs[1]);
	 if (SampleFreq <= 0) mexErrMsgTxt("error: invalid sample frequency.");

	 indx=0;

	 epsilon= (long) (0.1*SampleFreq)+1;
	 mrows = mxGetM(prhs[0]);
     ncols = mxGetN(prhs[0]);

	  // init waitbar
	 ntotal=mrows;
	 waitbar_step = ntotal/100;
     mxwaitbar_handle=mxCreateDoubleScalar(1);
     mxwaitbar_index=mxCreateDoubleScalar(1);
     mxwaitbar_string=mxCreateString("Running QRS detection ...");
     mxParam[0]=mxwaitbar_index;
     mxParam[1]=mxwaitbar_string;
     prwaitbar_index = mxGetPr(mxwaitbar_index);
     if (prwaitbar_index == NULL)mexErrMsgTxt("error: unable to create wait bar.");

     *(prwaitbar_index)=0;
     mexCallMATLAB(1,&mxwaitbar_handle,2, &mxParam[0], "waitbar");


	 /* get pointer to input data */
	 ptr_yin = (short*) mxGetPr(prhs[0]);

     new_dims[1]=mrows;
     new_dims[0]=ncols;

	 plhs[0] = mxCreateDoubleMatrix(mrows, ncols, mxREAL);
	 ptr_yqrsout = mxGetPr(plhs[0]);
     if (ptr_yqrsout == NULL) mexErrMsgTxt("error: unable to create output array.");

	/* find min/max to convert to short 
	ymax= *(ptr_yin);
	ymin= *(ptr_yin);

	ptrdbl = ptr_yin;
		for (ii=0;ii<mrows;ii++) {
			if (*(ptrdbl) > ymax) ymax= *(ptrdbl);
				else if ( *(ptrdbl) < ymin) ymin= *(ptrdbl);
				ptrdbl++;
		}
		//if (ymax != ymin) yscale=4096./(ymax-ymin); else yscale=1.;
		//if (ymax != ymin) yscale=32000./(ymax-ymin); else yscale=1.;
    */

    // init QRS 
    InitQRS();
    iNrSample=0;
    // qrsdet loops through for mrows- epsilon samples
	QRSDet(0, mrows-epsilon);
	CloseQRS();



	 // reallocate (shrink) RR vector;
     // remember we allocated same size as input signal 
	 // limit size to number of samples found == last indx
	 new_dims[0]=1;
	 new_dims[1]=indx;
     mxSetM(plhs[0], indx);
 mexCallMATLAB(0,NULL,1, &mxwaitbar_handle, "close");

  } //else
//    _exit:   ;

}
