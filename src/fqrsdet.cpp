// FQRSDET.cpp : Defines the entry point for the console application.
//

#include <cstdio>

#include "qrsdet.h"
#include "qrsfilter.h"
#include "dataio.h"

/*--------------------------------------------------------------------*/
#define  ProgramTitel	"fqrsdet"
/*--------------------------------------------------------------------*/

int main (int argc, char *argv[])
{
	/* set sample freq */
	SampleFreq=250;
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

	// clean up buffers
	CloseQRS();

	printf("\nEnd of program.\n");
	
	return(1);
}

