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
 * $NCDId: @(#)soundlib.h,v 1.25 1995/12/06 01:25:22 greg Exp $
 */

#ifndef _SOUNDLIB_H_
#define _SOUNDLIB_H_

#include <stdio.h>
#include <audio/sound.h>

#define SOUNDLIB_VERSION	3

#ifndef _SOUNDLIB_C_
/*****************************************************************************
 * 			       PUBLIC VARIABLES			     	     *
 *****************************************************************************/
#if defined(WIN32) && !defined(__CYGWIN__)
#define AuSoundFileChunkSize		(*AuSoundFileChunkSize_p)
#define AuSoundPortDuration		(*AuSoundPortDuration_p)
#define AuSoundPortLowWaterMark		(*AuSoundPortLowWaterMark_p)
#define AuSoundPortHighWaterMark	(*AuSoundPortHighWaterMark_p)
#define AuSoundRestartHardwarePauses	(*AuSoundRestartHardwarePauses_p)
#define AuMonitorFormat			(*AuMonitorFormat_p)
#define AuMonitorRate			(*AuMonitorRate_p)
#endif /* WIN32 */

extern unsigned int
                AuSoundFileChunkSize,		/* size of chunks for
						 * reading/writing files */
                AuSoundPortDuration,		/* duration for
						 * imports/exports */
                AuSoundPortLowWaterMark,	/* low water mark for
						 * imports/exports (in
						 * percent of duration) */
                AuSoundPortHighWaterMark;	/* high water mark for
						 * imports/exports (in
						 * percent of duration) */
extern AuBool   AuSoundRestartHardwarePauses;	/* re-start a flow if the
						 * hardware can't keep up */
#endif						/* !_SOUNDLIB_C_ */

/*****************************************************************************
 *				  DATA TYPES				     *
 *****************************************************************************/

/* struct pre-declarations */
struct _AuSoundDataRec;

typedef void
(*AuSoundDataHandler)
(
    AuServer *,			/* server */
    struct _AuSoundDataRec *,   /* private handler data */
    AuUint32                    /* number of bytes to handle */
);

typedef void
(*AuSoundCallback)
(
    AuServer *,                 /* server */
    AuEventHandlerRec *,	/* which */
    AuEvent *,	                /* event */
    AuPointer	        	/* callback data */
);

typedef struct _AuSoundDataRec
{
    Sound           s;                          /* sound format description */
    int             freeSound;                  /* should it be freed? */
    AuFlowID        flow;                       /* flow */
    char           *buf;                        /* data buffer */
    AuPointer       callback_data;              /* private data for callback */
    int             loopCount,                  /* play count for buckets */
                    numBytes;                   /* number of bytes to read */
    AuSoundCallback callback;                   /* done callback */
    AuSoundDataHandler dataHandler,             /* data handler */
                       dataHandlerStop;         /* handler for stop state */
    AuUint32	    length;                     /* number of bytes recorded */
}               AuSoundDataRec, *AuSoundDataPtr;

/*****************************************************************************
 * 			       PUBLIC INTERFACES			     *
 *****************************************************************************/
_AUFUNCPROTOBEGIN

extern          AuBucketID
AuSoundCreateBucketFromFile(
			    AuServer *,		/* server */
			    const char *,	/* filename */
			    AuUint32,	/* access */
			    AuBucketAttributes **,	/* RETURN_attributes */
			    AuStatus *		/* RETURN_status */
);

extern          AuBool
AuSoundCreateFileFromBucket(
			    AuServer *,		/* server */
			    const char *,	/* filename */
			    int,		/* file format */
			    AuBucketID,		/* bucket */
			    AuStatus *		/* RETURN_status */
);

extern          AuBucketID
AuSoundCreateBucketFromData(
			    AuServer *,		/* server */
			    Sound,		/* sound */
			    AuPointer,		/* data */
			    AuUint32,	/* access */
			    AuBucketAttributes **,	/* RETURN_attributes */
			    AuStatus *		/* RETURN_status */
);

extern AuPointer
AuSoundCreateDataFromBucket(
			    AuServer *,		/* server */
			    AuBucketID,		/* bucket */
			    Sound *,		/* RETURN_sound */
			    AuStatus *		/* RETURN_status */
);

extern AuEventHandlerRec *
AuSoundPlay(
		    AuServer *,			/* server */
		    AuDeviceID,			/* destination */
		    AuFixedPoint,		/* volume */
                    int,                        /* mode */
                    AuSoundDataPtr,             /* pointer to sound data */
		    AuFlowID *,			/* RETURN_flow */
		    int *,			/* RETURN_volume_mult_elem */
		    int *,			/* RETURN_monitor_element */
		    AuStatus *			/* RETURN_status */
);

extern AuEventHandlerRec *
AuSoundRecord(
		    AuServer *,			/* server */
		    AuDeviceID,			/* source */
		    AuFixedPoint,		/* gain */
                    AuUint32,		        /* num samples */
		    int,			/* mode */
                    AuSoundDataPtr,             /* pointer to sound data */
		    AuFlowID *,			/* RETURN_flow */
		    int *,			/* RETURN_volume_mult_elem */
		    AuStatus *			/* RETURN status */
);

extern AuEventHandlerRec *
AuSoundPlayFromFile(
		    AuServer *,			/* server */
		    const char *,		/* filename */
		    AuDeviceID,			/* destination */
		    AuFixedPoint,		/* volume */
		    AuSoundCallback,		/* done_callback */
		    AuPointer,			/* callback data */
		    AuFlowID *,			/* RETURN_flow */
		    int *,			/* RETURN_volume_mult_elem */
		    int *,			/* RETURN_monitor_element */
		    AuStatus *			/* RETURN_status */
);

extern AuEventHandlerRec *
AuSoundPlayFromData(
		    AuServer *,			/* server */
		    Sound,			/* sound */
		    AuPointer,			/* data */
		    AuDeviceID,			/* destination */
		    AuFixedPoint,		/* volume */
		    AuSoundCallback,		/* done_callback */
		    AuPointer,			/* callback data */
		    AuFlowID *,			/* RETURN_flow */
		    int *,			/* RETURN_volume_mult_elem */
		    int *,			/* RETURN_monitor_element */
		    AuStatus *			/* RETURN_status */
);

extern AuEventHandlerRec *
AuSoundRecordToData(
		    AuServer *,			/* server */
		    Sound,			/* sound */
		    AuPointer,			/* data */
		    AuDeviceID,			/* destination */
		    AuFixedPoint,		/* volume */
		    AuSoundCallback,		/* done_callback */
		    AuPointer,			/* callback data */
		    int,			/* line mode */
		    AuFlowID *,			/* RETURN_flow */
		    int *,			/* RETURN_volume_mult_elem */
		    AuStatus *			/* RETURN_status */
);

extern AuEventHandlerRec *
AuSoundRecordToFile(
		    AuServer *,			/* server */
		    const char *,		/* filename */
		    AuDeviceID,			/* source */
		    AuFixedPoint,		/* gain */
		    AuSoundCallback,		/* done_callback */
		    AuPointer,			/* callback data */
		    int,			/* mode */
		    int,			/* file format */
		    char *,			/* comment */
		    AuUint32,		/* rate */
		    int,			/* data format */
		    AuFlowID *,			/* RETURN_flow */
		    int *,			/* RETURN_volume_mult_elem */
		    AuStatus *			/* RETURN status */
);

extern AuEventHandlerRec *
AuSoundRecordToFileN(
		    AuServer *,			/* server */
		    const char *,		/* filename */
		    AuDeviceID,			/* source */
		    AuFixedPoint,		/* gain */
                    AuUint32,		        /* num samples */	
		    AuSoundCallback,		/* done_callback */
		    AuPointer,			/* callback data */
		    int,			/* mode */
		    int,			/* file format */
		    char *,			/* comment */
		    AuUint32,		/* rate */
		    int,			/* data format */
                    AuFlowID *,			/* RETURN_flow */
		    int *,			/* RETURN_volume_mult_elem */
		    AuStatus *			/* RETURN status */
);

extern          AuBool
AuSoundPlaySynchronousFromFile(
			       AuServer *,	/* server */
			       const char *,	/* filename */
			       int		/* volume */
);

extern AuEventHandlerRec *
AuSoundRecordToBucket(
		      AuServer *,		/* server */
		      AuBucketID,		/* destination */
		      AuDeviceID,		/* source */
		      AuFixedPoint,		/* gain */
		      AuSoundCallback,		/* done_callback */
		      AuPointer,		/* callback data */
		      int,			/* mode */
		      AuFlowID *,		/* RETURN_flow */
		      int *,			/* RETURN_volume_mult_elem */
		      AuStatus *		/* RETURN status */
);

extern AuEventHandlerRec *
AuSoundPlayFromBucket(
		      AuServer *,		/* server */
		      AuBucketID,		/* source */
		      AuDeviceID,		/* destination */
		      AuFixedPoint,		/* volume */
		      AuSoundCallback,		/* done_callback */
		      AuPointer,		/* callback data */
		      int,			/* loop count */
		      AuFlowID *,		/* RETURN_flow */
		      int *,			/* RETURN_volume_mult_elem */
		      int *,			/* RETURN_monitor_element */
		      AuStatus *		/* RETURN_status */
);

extern AuUint32
AuSoundRecordToDataLength(
		    AuEventHandlerRec *		/* which */
);
_AUFUNCPROTOEND


#endif						/* !_SOUNDLIB_H_ */
