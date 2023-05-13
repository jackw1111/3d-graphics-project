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
 * $NCDId: @(#)voc.h,v 1.9 1995/05/23 23:39:36 greg Exp $
 */

#ifndef _VOC_H_
#define _VOC_H_

#include <audio/audio.h>			/* for AuInt32 and AuUint32 */

#ifndef _VocConst
#define _VocConst const
#endif						/* _VocConst */

#ifndef _FUNCPROTOBEGIN
#ifdef __cplusplus			/* for C++ V2.0 */
#define _FUNCPROTOBEGIN extern "C" {	/* do not leave open across includes */
#define _FUNCPROTOEND }
#else
#define _FUNCPROTOBEGIN
#define _FUNCPROTOEND
#endif
#endif /* _FUNCPROTOBEGIN */

#define VOC_TERMINATOR		0
#define VOC_DATA		1
#define VOC_CONTINUE		2
#define VOC_SILENCE		3
#define VOC_MARKER		4
#define VOC_TEXT		5
#define VOC_REPEAT		6
#define VOC_REPEAT_END		7
#define VOC_EXTENDED		8

#define VOC_ID			"Creative Voice File\032"
#define VOC_ID_SIZE		20
#define VOC_DATA_OFFSET		26
#define	VOC_VERSION		0x10a
#define	VOC_VERSION_CHK		((~VOC_VERSION + 0x1234) & 0xffff)

typedef struct
{
    FILE           *fp;
    char           *comment;
    AuUint32        sampleRate,
                    dataOffset,
                    dataSize;
    unsigned int    compression,
                    tracks;

    /* private data */
    unsigned int    writing;
}               VocInfo;

/*****************************************************************************
 *			       PUBLIC INTERFACES			     *
 *****************************************************************************/

_FUNCPROTOBEGIN

extern VocInfo *
VocOpenFileForReading(
		      const char *		/* file name */
);

extern VocInfo *
VocOpenFileForWriting(
		      const char *,		/* file name */
		      VocInfo *			/* info */
);

extern int
VocCloseFile(
	     VocInfo *				/* info */
);

extern int
VocReadFile(
	    char *,				/* buffer */
	    int,				/* num bytes */
	    VocInfo *				/* info */
);

extern int
VocWriteFile(
	     char *,				/* buffer */
	     int,				/* num bytes */
	     VocInfo *				/* info */
);

extern int
VocSeekFile(
	      int,                              /* number of audio bytes */
	      VocInfo *				/* info */
);

extern int
VocTellFile(
	      VocInfo *				/* info */
);

extern int
VocFlushFile(
	      VocInfo *				/* info */
);

extern int
VocRewindFile(
	      VocInfo *				/* info */
);

_FUNCPROTOEND

#endif						/* _VOC_H_ */
