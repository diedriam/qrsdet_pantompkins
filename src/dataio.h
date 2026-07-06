#ifndef DATAIO_H
#define DATAIO_H

/*----------------------------------------------------------------------
	dataio.h
	specific routines for data reading and
	specific routine for output of RRResult
----------------------------------------------------------------------*/

#include <stdio.h>

#include "qrsdet.h"

extern FILE	*SourceFile, *RRFile;
extern long	actblocknr;
extern short	buf[256];

int		ReadBlock(long iblocknr, int signalnumber);
long	GetMaxNrSample(void);
void	ReadSample(long index, int channelnumber, short* channel);

#endif /* DATAIO_H */
