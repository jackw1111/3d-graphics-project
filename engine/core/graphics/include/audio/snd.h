/*
 * <audio/snd.h>
 * 
 * This file contains interfaces to the .snd files.  It is not dependent on the
 * NCD-AUDIO service and can easily be used for other purposes.
 * 
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
 * $NCDId: @(#)snd.h,v 1.13 1995/05/23 23:37:12 greg Exp $
 */

#ifndef _SND_H_
#define _SND_H_

#include <stdio.h>
#include <audio/audio.h>	/* for AuInt32 and AuUint32 */

#ifndef _SndConst
#define _SndConst const
#endif						/* _SndConst */

#ifndef _FUNCPROTOBEGIN
#ifdef __cplusplus			/* for C++ V2.0 */
#define _FUNCPROTOBEGIN extern "C" {	/* do not leave open across includes */
#define _FUNCPROTOEND }
#else
#define _FUNCPROTOBEGIN
#define _FUNCPROTOEND
#endif
#endif /* _FUNCPROTOBEGIN */

/*****************************************************************************
 *				  DATA TYPES				     *
 *****************************************************************************/

#define SND_MAGIC_NUM				0x2e736e64	/* .snd */
#define SND_DATA_SIZE_UNKNOWN			0xffffffff

#define SND_FORMAT_UNSPECIFIED			0
#define SND_FORMAT_ULAW_8			1
#define SND_FORMAT_LINEAR_8			2
#define SND_FORMAT_LINEAR_16			3
#define SND_FORMAT_LINEAR_24			4
#define SND_FORMAT_LINEAR_32			5
#define SND_FORMAT_FLOAT_32			6
#define SND_FORMAT_FLOAT_64			7
#define SND_FORMAT_ULAW_COMPRESSED_8		23

typedef struct _SndHeader
{
    AuUint32   magic;			/* the value 0x2e736e64
						 * (ASCII ".snd") */
    AuUint32   dataOffset;			/* the offset, in octets, to
						 * the data part. The minimum
						 * valid number is 24
						 * (decimal). */
    AuUint32   dataSize;			/* the size in octets, of the
						 * data part. If unknown, the
						 * value 0xffffffff should be
						 * used. */
    /** the data format:
     *
     * value    format
     *   1      8-bit ISDN u-law
     *   2      8-bit linear PCM [REF-PCM]
     *   3      16-bit linear PCM
     *   4      24-bit linear PCM
     *   5      32-bit linear PCM
     *   6      32-bit IEEE floating point
     *   7      64-bit IEEE floating point
     *  23      8-bit ISDN u-law compressed
     *          using the CCITT G.721 ADPCM
     *          voice data format scheme.
     */
    AuUint32   format;

    AuUint32   sampleRate;			/* the number of
						 * samples/second (e.g.,
						 * 8000) */
    AuUint32   tracks;			/* the number of interleaved
						 * tracks (e.g., 1) */
}               SndHeader;

typedef struct
{
    SndHeader       h;
    char           *comment;
    FILE           *fp;
    int             writing;
}               SndInfo;

/*****************************************************************************
 *			       PUBLIC INTERFACES			     *
 *****************************************************************************/

_FUNCPROTOBEGIN

extern SndInfo *
SndOpenFileForReading(
		      const char *		/* file name */
);

extern SndInfo *
SndOpenFileForWriting(
		      const char *,		/* file name */
		      SndInfo *			/* info */
);

extern int
SndCloseFile(
	     SndInfo *				/* info */
);

extern int
SndReadFile(
	    char *,				/* buffer */
	    int,				/* num bytes */
	    SndInfo *				/* info */
);

extern int
SndWriteFile(
	     char *,				/* buffer */
	     int,				/* num bytes */
	     SndInfo *				/* info */
);

extern int
SndSeekFile(
	      int,                              /* number of audio bytes */
	      SndInfo *				/* info */
);

extern int
SndTellFile(
	      SndInfo *				/* info */
);

extern int
SndFlushFile(
	      SndInfo *				/* info */
);

extern int
SndRewindFile(
	      SndInfo *				/* info */
);

_FUNCPROTOEND

#endif						/* _SND_H_ */
