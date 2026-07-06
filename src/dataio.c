/*----------------------------------------------------------------------
	dataio.c
	specific routines for data reading and
	specific routine for output of RRResult;
----------------------------------------------------------------------*/

#include "dataio.h"

FILE	*SourceFile, *RRFile;
long	actblocknr=-1;
short	buf[256];

/*----------------------------------------------------------------------
	Specific routines for Data File Reading
	
----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
    ReadBlock
	Read one data block for specific data file structure
	interleaved signals of size short (int16) 
----------------------------------------------------------------------*/

#define	DATAFIRSTSKIP 0L
#define DATABLOCKSIZE 512L
#define SAMPLESPERBLK 256 
#define MAXCHANNEL    1


int	ReadBlock(long iblocknr, int signalnumber)
{
	long	loffset;
	int	i;
	long idx;
	unsigned char	bytebuf[DATABLOCKSIZE];
	short 	*ptr_buf = buf;

	loffset = (long) iblocknr;
	loffset*= DATABLOCKSIZE;
	if ((fseek(SourceFile, loffset+DATAFIRSTSKIP, SEEK_SET)) == -1) return -1;

	if (fread(bytebuf, sizeof(bytebuf), 1, SourceFile) < 1) return -1;

	for (i=0; i<SAMPLESPERBLK; i++)
	{
		idx = 2*(i*MAXCHANNEL+signalnumber);
		*ptr_buf++ = bytebuf[idx] + (short)(bytebuf[idx+1]<<8);
	}
	actblocknr = iblocknr;
	return SAMPLESPERBLK;
}

/*----------------------------------------------------------------------
    Get the maximal number of samples 
----------------------------------------------------------------------*/
long GetMaxNrSample(void)
{
long nbytes;
	fseek(SourceFile, 0L, SEEK_END);
	nbytes = ftell(SourceFile)/DATABLOCKSIZE* SAMPLESPERBLK;
	fseek(SourceFile, 0L, SEEK_SET);
  return(nbytes);
}

/*--------------------------------------------------------------------
   ReadSample
	get one sample of the given channel by absolute sample index,
	reading a new block only when the index crosses a block boundary
--------------------------------------------------------------------*/
void ReadSample(long index, int channelnumber, short* channel)
{
long	iblock;
int	i;
	iblock = index/SAMPLESPERBLK;
	i = (int)(index % SAMPLESPERBLK);
	if (iblock != actblocknr) ReadBlock(iblock, channelnumber);
	*channel = buf[i];
}


/*--------------------------------------------------------------------
    GetSample
	Get the next sample from the source file (single-channel format,
	MAXCHANNEL is 1, so the channel index is 0) and advance iNrSample
--------------------------------------------------------------------*/
short GetSample(void)
{
short ch;
	ReadSample(iNrSample++, 0, &ch);
	return ch;
}

/*----------------------------------------------------------------------
	Specific Routine for Output of RRResult;
----------------------------------------------------------------------*/
void	PutRRRes(RRMsgForm RRRes)
{
	/* output in ASCII format (sample units) */
	// todo find out where the offset -20 comes from
	fprintf(RRFile,"%ld\t%ld\t%d\n",
	   RRRes.iNr-20, RRRes.RRI, RRRes.Test);
}


