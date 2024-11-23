// FQRSDET.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "math.h"
#include "stdio.h"

#define ANSI_C

#ifndef ANSI_C
	#define SEEK_SET 0
	#define SEEK_CUR 1
	#define SEEK_END 2
#endif

#include "qrsdet.h"
#include "qrsfilter.c"
#include "dataio.c"
#include "qrsdet.c"

/*--------------------------------------------------------------------*/
#define  ProgramTitel	"fqrsdet"
/*--------------------------------------------------------------------*/


#ifdef ANSI_C
int main (int argc, char *argv[])
#endif
#ifndef ANSI_C
main (argc, argv)
	int 	argc;
	char *argv[];
#endif
{
	/* set sample freq */
	SampleFreq=250;	
	char	line[80];
    long	epochstart, epochstop, MaxNrEpoch;

    printf("\n\nSTANDALONE  %s\n", ProgramTitel);
   	
    if (argc != 4){
	   printf("\nUsage of program: fqrsdet sourcefile targetfile samplefreq\n");
	   return(0);
    }
    sscanf(argv[3],"%lg",&SampleFreq);
    printf("Input sample frequency is %g", SampleFreq);

   if ((SourceFile =fopen(argv[1], "rb"))== NULL){
		printf ("\nUnable to open source file!\n");
		return(0);
   }		
   if ((RRFile = fopen(argv[2], "wb")) == NULL){
		printf("\nUnable to open target file!\n");
		fclose(SourceFile);
		return(0);   
   }	

   // could use start and stop as argument
   // lets analyze whole file for now	
   MaxNrEpoch = GetMaxNrSample()/(60*SampleFreq);
   epochstart = 0;	
   epochstop = MaxNrEpoch;
		
   // init buffers and filters
   InitQRS();
  
   // QRSDet loops from start to end and 
   // uses qrsdata where io routines for get and put data are defined
   QRSDet(epochstart*SampleFreq*60, (epochstop+1)*SampleFreq*60);
			
	// close files 
	fclose(SourceFile);
	fclose(RRFile);

	// clean up	buffers
	if (LastIn != NULL) delete [] LastIn;
	if (LastFilt != NULL) delete [] LastFilt;
	if (LastDiff2 != NULL) delete [] LastDiff2;
	if (filtcoef != NULL) delete [] filtcoef;

	printf("\nEnd of program.\n");
	
	return(1);
}

