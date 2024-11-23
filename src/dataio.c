/*----------------------------------------------------------------------
	dataio
	specific routines for data reading and
	specific routine for output of RRResult;
----------------------------------------------------------------------*/
int		ReadBlock(long iblocknr);
long	GetMaxNrSample(void);
void	ReadSample(long index, short* channel);

short	GetSample(void);
void	PutRRRes(RRMsgForm RRRes);

/*--------------------------------------------------------------------*/

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
	int	i, nelement = 1;
	long idx;
	unsigned char	bytebuf[DATABLOCKSIZE]; 
	unsigned char	*ptr_bytebuf, *ptr_help= bytebuf;
	short 	*ptr_buf = buf;

	loffset = (long) iblocknr;
	loffset*= DATABLOCKSIZE;
	if ((fseek(SourceFile, loffset+DATAFIRSTSKIP, SEEK_SET)) == -1) return -1;
	
	nelement = fread(bytebuf, sizeof(bytebuf), 1, SourceFile);
	if (nelement <1)  return -1;

	for (i=0; i<SAMPLESPERBLK; i++)
	{
		idx = 2*(i*MAXCHANNEL+signalnumber);
		//*ptr_buf++ = bytebuf[idx]+
		// 	256*bytebuf[idx+1];
		*ptr_buf++ = bytebuf[idx] + (short)(bytebuf[idx+1]<<8);
	}
	actblocknr = iblocknr;
	return nelement/2;
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
   GetSample
	get a sample of channel 1 & 2
	index nr. of sample in file
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
	Get a new sample from input file
	summe of channel 1 and 2
	inversion of the result		 -(ch1+ch2)/2
	store the last sample in temporary input buffer LastIn
--------------------------------------------------------------------*/

short GetSample(void)
{
short ch;
	// read channel 1
	ReadSample(iNrSample++, 1, &ch);
	return ch;
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

	/* output in ASCII format (sampleunits)*/
	// todo find out where the offset -20 comes from
	fprintf(RRFile,"%ld\t%ld\t%d\n",
	   RRRes.iNr-20, RRRes.RRI, RRRes.Test);
}


