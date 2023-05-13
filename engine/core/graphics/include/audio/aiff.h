/*
 * Copyright 1993 Network Computing Devices, Inc.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name Network Computing Devices, Inc. not be
 * used in advertising or publicity pertaining to distribution of this
 * software without specific, written prior permission.
 * 
 * THIS SOFTWARE IS PROVIDED 'AS-IS'.  NETWORK COMPUTING DEVICES, INC.,
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT
 * LIMITATION ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NONINFRINGEMENT.  IN NO EVENT SHALL NETWORK
 * COMPUTING DEVICES, INC., BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS OF USE, DATA,
 * OR PROFITS, EVEN IF ADVISED OF THE POSSIBILITY THEREOF, AND REGARDLESS OF
 * WHETHER IN AN ACTION IN CONTRACT, TORT OR NEGLIGENCE, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * $NCDId: @(#)aiff.h,v 1.4 1995/05/23 23:36:26 greg Exp $
 */

#ifndef _AIFF_H_
#define _AIFF_H_

#include <audio/audio.h>	/* for AuInt32 and AuUint32 */

#ifndef _AiffConst
#define _AiffConst const
#endif						/* _AiffConst */

#ifndef _FUNCPROTOBEGIN
#ifdef __cplusplus			/* for C++ V2.0 */
#define _FUNCPROTOBEGIN extern "C" {	/* do not leave open across includes */
#define _FUNCPROTOEND }
#else
#define _FUNCPROTOBEGIN
#define _FUNCPROTOEND
#endif
#endif /* _FUNCPROTOBEGIN */

#define AIFF_FormID			"FORM"
#define AIFF_AiffID			"AIFF"
#define AIFF_CommonID			"COMM"
#define AIFF_SoundDataID		"SSND"
#define AIFF_CommentID			"COMT"

#define AIFF_SizeofExtended		10
#define AIFF_SizeofCommentChunk		10
#define AIFF_SizeofCommonChunk		(8 + AIFF_SizeofExtended)
#define AIFF_SizeofSoundDataChunk	8

typedef AuUint32 AIFF_ID;
typedef short   AIFF_MARKER_ID;

typedef struct
{
    AIFF_ID         ckID;			/* chunk ID */
    AuInt32            ckSize;			/* chunk data size, in bytes */
}               AiffChunk;

typedef struct
{
    FILE           *fp;
    char           *comment;
    short           channels,
                    bitsPerSample;
    AuInt32            sampleRate;
    AuUint32   dataOffset,
                    numSamples;

    /* private data */
    AuUint32   fileSize,
                    dataSize,
                    sizeOffset;
    unsigned int    writing;
}               AiffInfo;

/*****************************************************************************
 *			       PUBLIC INTERFACES			     *
 *****************************************************************************/

_FUNCPROTOBEGIN

extern AiffInfo *
AiffOpenFileForReading(
		       const char *		/* file name */
);

extern AiffInfo *
AiffOpenFileForWriting(
		       const char *,		/* file name */
		       AiffInfo *		/* info */
);

extern int
AiffCloseFile(
	      AiffInfo *			/* info */
);

extern int
AiffReadFile(
	     char *,				/* buffer */
	     int,				/* num bytes */
	     AiffInfo *				/* info */
);

extern int
AiffWriteFile(
	      char *,				/* buffer */
	      int,				/* num bytes */
	      AiffInfo *			/* info */
);

extern int
AiffSeekFile(
	      int,                              /* number of audio bytes */
	      AiffInfo *			/* info */
);

extern int
AiffTellFile(
	      AiffInfo *			/* info */
);

extern int
AiffFlushFile(
	      AiffInfo *			/* info */
);

extern int
AiffRewindFile(
	       AiffInfo *			/* info */
);

_FUNCPROTOEND

#endif						/* _AIFF_H_ */
