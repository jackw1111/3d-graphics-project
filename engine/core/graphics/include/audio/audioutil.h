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
 * $NCDId: @(#)audioutil.h,v 1.21 1995/12/28 19:47:25 greg Exp $
 * 
 * <audio/audioutil.h>
 * 
 * This file contains utilities for using the audio library.
 */

#ifndef AUDIOUTIL_H_
#define AUDIOUTIL_H_

_AUFUNCPROTOBEGIN

/*****************************************************************************
 *				 FLOW UTILITIES				     *
 *****************************************************************************/

extern          AuFlowID AUCDECL
AuGetScratchFlow(
		 AuServer *,			/* server */
		 AuStatus *			/* RETURN_status */
);

extern void AUCDECL
AuReleaseScratchFlow(
		     AuServer *,		/* server */
		     AuFlowID,			/* flow */
		     AuStatus *			/* RETURN_status */
);

extern          AuFlowID AUCDECL
AuGetScratchFlowToBucket(
			 AuServer *,		/* server */
			 AuBucketID,		/* bucket */
			 int *,			/* RETURN_import */
			 AuStatus *		/* RETURN_status */
);

extern          AuFlowID AUCDECL
AuGetScratchFlowFromBucket(
			   AuServer *,		/* server */
			   AuBucketID,		/* bucket */
			   int *,		/* RETURN_export */
			   AuStatus *		/* RETURN_status */
);

extern void AUCDECL
AuStartFlow(
	   AuServer *,				/* server */
	   AuFlowID,				/* flow */
	   AuStatus *				/* RETURN_status */
);

extern void AUCDECL
AuStopFlow(
	   AuServer *,				/* server */
	   AuFlowID,				/* flow */
	   AuStatus *				/* RETURN_status */
);

extern void AUCDECL
AuPauseFlow(
	   AuServer *,				/* server */
	   AuFlowID,				/* flow */
	   AuStatus *				/* RETURN_status */
);

/*****************************************************************************
 *			       EVENT DISPATCHING			     *
 *****************************************************************************/

extern void AUCDECL
AuHandleEvents(
	       AuServer *			/* server */
);

extern          AuBool AUCDECL
AuDispatchEvent(
		AuServer *,			/* server */
		AuEvent *
);

extern AuEventHandlerRec * AUCDECL
AuRegisterEventHandler(
		       AuServer *,		/* server */
		       AuMask,			/* value_mask */
		       int,			/* type */
		       AuID,			/* id */
		       AuEventHandlerCallback,	/* callback */
		       AuPointer		/* data */
);

extern void AUCDECL
AuUnregisterEventHandler(
			 AuServer *,		/* server */
			 AuEventHandlerRec *	/* handler */
);

extern AuEventHandlerRec * AUCDECL
AuLookupEventHandler(
		     AuServer *,		/* server */
		     AuEvent *,			/* event */
		     AuEventHandlerRec *	/* startwith */
);

/*****************************************************************************
 *			       STRING UTILITIES				     *
 *****************************************************************************/

const char  * AUCDECL
AuFormatToString(
		 unsigned int			/* format */
);

int AUCDECL
AuStringToFormat(
		 const char *			/* string */
);

const char  * AUCDECL
AuFormatToDefine(
		 unsigned int			/* format */
);

int AUCDECL
AuDefineToFormat(
		 const char *			/* define */
);

const char  * AUCDECL
AuWaveFormToString(
		 unsigned int			/* waveform */
);

int AUCDECL
AuStringToWaveForm(
		 const char *			/* string */
);

/*****************************************************************************
 *			     DATA CONVERSION UTILITIES			     *
 *****************************************************************************/

int AUCDECL
AuConvertDataToShort(
		     int,			/* data format */
		     int,			/* num bytes */
		     AuPointer			/* data */
);

int AUCDECL
AuConvertShortToData(
		     int,			/* data format */
		     int,			/* num bytes */
		     AuPointer			/* data */
);

/*****************************************************************************
 *			       	     MISC				     *
 *****************************************************************************/

AuEventHandlerRec * AUCDECL
AuMonitorDevice(
		AuServer *,			/* server */
		int,				/* sample rate */
		AuDeviceID,			/* input device */
		AuDeviceID,			/* output device */
		AuFixedPoint,			/* volume */
		void (*) (			/* done_callback */
			  AuServer *,		/* server */
			  AuEventHandlerRec *,	/* which */
			  AuEvent *,		/* event */
			  AuPointer		/* callback data */
			  ),
		AuPointer,			/* callback data */
		AuFlowID *,			/* RETURN_flow */
		int *,				/* RETURN_volume_mult_element */
		int *,				/* RETURN_monitor_element */
		AuStatus *			/* RETURN_status */
);

_AUFUNCPROTOEND

#endif						/* AUDIOUTIL_H_ */
