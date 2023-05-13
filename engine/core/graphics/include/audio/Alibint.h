/*
 * Copyright 1993 Network Computing Devices, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
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
 * $NCDId: @(#)Alibint.h,v 1.20 1995/12/28 19:44:51 greg Exp $
 */

/* Portions derived from */
/* $XConsortium: Xlibint.h,v 11.96 92/01/30 10:22:57 rws Exp $ */
/* Copyright 1984, 1985, 1987, 1989  Massachusetts Institute of Technology */

/*
Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  M.I.T. makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.
*/

/*
 *	Xlibint.h - Header definition and support file for the internal
 *	support routines used by the C subroutine interface
 *	library (Xlib) to the X Window System.
 *
 *	Warning, there be dragons here....
 */

#ifndef _NCD_ALIBINT_H_
#define _NCD_ALIBINT_H_

#include <audio/audiolib.h>

/* round up to a four byte boundary */
#define PAD4(x)				(((x) + 3) & ~3)

/* useful macros for transfering attributes */
#define _xfer(s, d, x)	(d).x = (s).x

#define _xferCommonPart(s, d)						       \
{									       \
    _xfer(s, d, value_mask);						       \
    _xfer(s, d, changable_mask);					       \
    _xfer(s, d, id);							       \
    _xfer(s, d, kind);							       \
    _xfer(s, d, use);							       \
    _xfer(s, d, format);						       \
    _xfer(s, d, num_tracks);						       \
    _xfer(s, d, access);						       \
    _xfer((s).description, (d).description, type);			       \
    _xfer((s).description, (d).description, len);			       \
}

#define _xferDevicePart(s, d)						       \
{									       \
    _xfer(s, d, min_sample_rate);					       \
    _xfer(s, d, max_sample_rate);					       \
    _xfer(s, d, location);						       \
    _xfer(s, d, gain);							       \
    _xfer(s, d, line_mode);						       \
    _xfer(s, d, num_children);						       \
}

#define _xferBucketPart(s, d)						       \
{									       \
    _xfer(s, d, sample_rate);						       \
    _xfer(s, d, num_samples);						       \
}

#define _xferDeviceAttributes(s, d)					       \
{									       \
    _xferCommonPart((s)->common, (d).common);				       \
    _xferDevicePart((s)->device, (d).device);				       \
}

#define _xferBucketAttributes(s, d)					       \
{									       \
    _xferCommonPart((s)->common, (d).common);				       \
    _xferBucketPart((s)->bucket, (d).bucket);				       \
}

/*
 * define the following if you want the _AuData macro to be a procedure instead
 */
#ifdef CRAY
#define _AuDataRoutineIsProcedure
#endif /* CRAY */

/*
 * _QEvent datatype for use in input queueing.
 */
typedef struct _AuSQEvent {
    struct _AuSQEvent *next;
    AuEvent event;
} _AuQEvent;

#include <stdio.h>
#include <audio/Aproto.h>
#include <errno.h>
#define _AUBCOPYFUNC _Aubcopy
#include <audio/Afuncs.h>
#include <audio/Aosdefs.h>

/* Utek leaves kernel macros around in include files (bleah) */
#ifdef dirty
#undef dirty
#endif

#ifdef CRAY
#define WORD64
#endif

#ifndef AU_NOT_STDC_ENV
# include <stdlib.h>
# include <string.h>
#else
# ifndef hpux
   extern char *malloc(), *realloc(), *calloc();
# endif
void exit();
#ifdef SYSV
#include <string.h>
#else
#include <strings.h>
#endif
#endif
#if defined(macII) && !defined(__STDC__)  /* stdlib.h fails to define these */
extern char *malloc(), *realloc(), *calloc();
#endif /* macII */

/*
 * The following definitions can be used for locking requests in multi-threaded
 * address spaces.
 */
#include "mutex.h"

#define Aufree(ptr) free((ptr))

/*
 * Note that some machines do not return a valid pointer for malloc(0), in
 * which case we provide an alternate under the control of the
 * define MALLOC_0_RETURNS_NULL.  This is necessary because some
 * Aulib code expects malloc(0) to return a valid pointer to storage.
 */
#ifdef MALLOC_0_RETURNS_NULL

# define Aumalloc(size) malloc(((size) > 0 ? (size) : 1))
# define Aurealloc(ptr, size) realloc((ptr), ((size) > 0 ? (size) : 1))
# define Aucalloc(nelem, elsize) calloc(((nelem) > 0 ? (nelem) : 1), (elsize))

#else

# define Aumalloc(size) malloc((size))
# define Aurealloc(ptr, size) realloc((ptr), (size))
# define Aucalloc(nelem, elsize) calloc((nelem), (elsize))

#endif /* MALLOC_0_RETURNS_NULL */

#ifndef NULL
#define NULL 0
#endif
#define LOCKED 1
#define UNLOCKED 0

#ifdef notdef
extern int errno;			/* Internal system error number. */
#endif

#ifndef BUFSIZE
#define BUFSIZE 2048			/* Au output buffer size. */
#endif

/*
 * display flags
 */
#define AuServerFlagsIOError	(1L << 0)
#define AuServerFlagsClosing	(1L << 1)

/*
 * Au Protocol packetizing macros.
 */

/*   Need to start requests on 64 bit word boundries
 *   on a CRAY computer so add a NoOp (127) if needed.
 *   A character pointer on a CRAY computer will be non-zero
 *   after shifting right 61 bits of it is not pointing to
 *   a word boundary.
 */
#ifdef WORD64
#define WORD64ALIGN if ((AuInt32)aud->bufptr >> 61) {\
           aud->last_req = aud->bufptr;\
           *(aud->bufptr)   = Au_NoOperation;\
           *(aud->bufptr+1) =  0;\
           *(aud->bufptr+2) =  0;\
           *(aud->bufptr+3) =  1;\
             aud->request++;\
             aud->bufptr += 4;\
         }
#else /* else does not require alignment on 64-bit boundaries */
#define WORD64ALIGN
#endif /* WORD64 */


/*
 * _AuGetReq - Get the next avilable Au request packet in the buffer and
 * return it. 
 *
 * "name" is the name of the request, e.g. CreatePixmap, OpenFont, etc.
 * "req" is the name of the request pointer.
 *
 */
#ifdef UNIXCPP
#error "bummer"
#endif
#if defined(__STDC__) && !defined(UNIXCPP)
#define _AuGetReq(name, req, aud) \
        WORD64ALIGN\
	if ((aud->bufptr + SIZEOF(au##name##Req)) > aud->bufmax)\
		_AuFlush(aud);\
	req = (au##name##Req *)(aud->last_req = aud->bufptr);\
	req->reqType = Au_##name;\
	req->length = (SIZEOF(au##name##Req))>>2;\
	aud->bufptr += SIZEOF(au##name##Req);\
	aud->request++

#else  /* non-ANSI C uses empty comment instead of "##" for token concatenation */
#define _AuGetReq(name, req, aud) \
        WORD64ALIGN\
	if ((aud->bufptr + SIZEOF(au/**/name/**/Req)) > aud->bufmax)\
		_AuFlush(aud);\
	req = (au/**/name/**/Req *)(aud->last_req = aud->bufptr);\
	req->reqType = Au_/**/name;\
	req->length = (SIZEOF(au/**/name/**/Req))>>2;\
	aud->bufptr += SIZEOF(au/**/name/**/Req);\
	aud->request++
#endif

/* _AuGetReqExtra is the same as _AuGetReq, but allocates "n" additional
   bytes after the request. "n" must be a multiple of 4!  */

#if  defined(__STDC__) && !defined(UNIXCPP)
#define _AuGetReqExtra(name, n, req, aud) \
        WORD64ALIGN\
	if ((aud->bufptr + SIZEOF(au##name##Req) + n) > aud->bufmax)\
		_AuFlush(aud);\
	req = (au##name##Req *)(aud->last_req = aud->bufptr);\
	req->reqType = Au_##name;\
	req->length = (SIZEOF(au##name##Req) + n)>>2;\
	aud->bufptr += SIZEOF(au##name##Req) + n;\
	aud->request++
#else
#define _AuGetReqExtra(name, n, req, aud) \
        WORD64ALIGN\
	if ((aud->bufptr + SIZEOF(au/**/name/**/Req) + n) > aud->bufmax)\
		_AuFlush(aud);\
	req = (au/**/name/**/Req *)(aud->last_req = aud->bufptr);\
	req->reqType = Au_/**/name;\
	req->length = (SIZEOF(au/**/name/**/Req) + n)>>2;\
	aud->bufptr += SIZEOF(au/**/name/**/Req) + n;\
	aud->request++
#endif


/*
 * _AuGetResReq is for those requests that have a resource ID 
 * (Window, Pixmap, GContext, etc.) as their single argument.
 * "rid" is the name of the resource. 
 */

#if  defined(__STDC__) && !defined(UNIXCPP)
#define _AuGetResReq(name, rid, req, aud) \
        WORD64ALIGN\
	if ((aud->bufptr + SIZEOF(auResourceReq)) > aud->bufmax)\
	    _AuFlush(aud);\
	req = (auResourceReq *) (aud->last_req = aud->bufptr);\
	req->reqType = Au_##name;\
	req->length = 2;\
	req->id = (rid);\
	aud->bufptr += SIZEOF(auResourceReq);\
	aud->request++
#else
#define _AuGetResReq(name, rid, req, aud) \
        WORD64ALIGN\
	if ((aud->bufptr + SIZEOF(auResourceReq)) > aud->bufmax)\
	    _AuFlush(aud);\
	req = (auResourceReq *) (aud->last_req = aud->bufptr);\
	req->reqType = Au_/**/name;\
	req->length = 2;\
	req->id = (rid);\
	aud->bufptr += SIZEOF(auResourceReq);\
	aud->request++
#endif

/*
 * _AuGetEmptyReq is for those requests that have no arguments
 * at all. 
 */
#if  defined(__STDC__) && !defined(UNIXCPP)
#define _AuGetEmptyReq(name, req, aud) \
        WORD64ALIGN\
	if ((aud->bufptr + SIZEOF(auReq)) > aud->bufmax)\
	    _AuFlush(aud);\
	req = (auReq *) (aud->last_req = aud->bufptr);\
	req->reqType = Au_##name;\
	req->length = 1;\
	aud->bufptr += SIZEOF(auReq);\
	aud->request++
#else
#define _AuGetEmptyReq(name, req, aud) \
        WORD64ALIGN\
	if ((aud->bufptr + SIZEOF(auReq)) > aud->bufmax)\
	    _AuFlush(aud);\
	req = (auReq *) (aud->last_req = aud->bufptr);\
	req->reqType = Au_/**/name;\
	req->length = 1;\
	aud->bufptr += SIZEOF(auReq);\
	aud->request++
#endif


#define _AuSyncHandle(aud) \
	if (aud->synchandler) _AuDoSyncHandle(aud)

/*
 * _AuData - Place data in the buffer and pad the end to provide
 * 32 bit word alignment.  Transmit if the buffer fills.
 *
 * "aud" is a pointer to a AuServer.
 * "data" is a pinter to a data buffer.
 * "len" is the length of the data buffer.
 * we can presume buffer less than 2^16 bytes, so bcopy can be used safely.
 */
#ifndef _AuDataRoutineIsProcedure
#define _AuData(aud, data, len) \
	if (aud->bufptr + (len) <= aud->bufmax) {\
		bcopy(data, aud->bufptr, (int)len);\
		aud->bufptr += ((len) + 3) & ~3;\
	} else\
		_AuSend(aud, data, len)
#endif /* _AuDataRoutineIsProcedure */


/* Allocate bytes from the buffer.  No padding is done, so if
 * the length is not a multiple of 4, the caller must be
 * careful to leave the buffer aligned after sending the
 * current request.
 *
 * "type" is the type of the pointer being assigned to.
 * "ptr" is the pointer being assigned to.
 * "n" is the number of bytes to allocate.
 *
 * Example: 
 *    xTextElt *elt;
 *    BufAlloc (xTextElt *, elt, nbytes)
 */

#define BufAlloc(type, ptr, n) \
    if (aud->bufptr + (n) > aud->bufmax) \
        _AuFlush (aud); \
    ptr = (type) aud->bufptr; \
    aud->bufptr += (n);

/*
 * provide emulation routines for smaller architectures
 */
#ifndef WORD64
#define _AuData16(aud, data, len) _AuData((aud), (char *)(data), (len))
#define _AuData32(aud, data, len) _AuData((aud), (char *)(data), (len))
#define _AuRead16Pad(aud, data, len) _AuReadPad((aud), (char *)(data), (len))
#define _AuRead16(aud, data, len) _AuRead((aud), (char *)(data), (len))
#define _AuRead32(aud, data, len) _AuRead((aud), (char *)(data), (len))
#endif /* not WORD64 */

#define _AuPackData16(aud,data,len) _AuData16 (aud, data, len)
#define _AuPackData32(aud,data,len) _AuData32 (aud, data, len)

/* Xlib manual is bogus */
#define _AuPackData(aud,data,len) _AuPackData16 (aud, data, len)

#define aumin(a,b) (((a) < (b)) ? (a) : (b))
#define aumax(a,b) (((a) > (b)) ? (a) : (b))

#ifdef MUSTCOPY

/* for when 32-bit alignment is not good enough */
#define _AuOneDataCard32(aud,dstaddr,srcvar) \
  { aud->bufptr -= 4; _AuData32 (aud, (char *) &(srcvar), 4); }

#else

/* srcvar must be a variable for large architecture version */
#define _AuOneDataCard32(aud,dstaddr,srcvar) \
  { *(AuUint32 *)(dstaddr) = (srcvar); }

#endif /* MUSTCOPY */

typedef struct _AuInternalAsync {
    struct _AuInternalAsync *next;
    AuBool (*handler)();
    AuPointer data;
} _AuAsyncHandler;

typedef struct _AuAsyncEState {
    AuUint32 min_sequence_number;
    AuUint32 max_sequence_number;
    unsigned char error_code;
    unsigned char major_opcode;
    unsigned short minor_opcode;
    unsigned char last_error_received;
    int error_count;
} _AuAsyncErrorState;

#define _AuEnqAsyncHandler(aud,hhh,ppp,ddd) { \
    (hhh)->next = (aud)->async_handlers; \
    (hhh)->handler = (ppp); \
    (hhh)->data = (AuPointer) (ddd); \
    (aud)->async_handlers = (hhh); \
    }

#define _AuDeqAsyncHandler(aud,handler) { \
    if (aud->async_handlers == (handler)) \
	aud->async_handlers = (handler)->next; \
    else \
	_AuDoDeqAsyncHandler(aud, handler); \
    }


/*
 * This structure is private to the library.
 */
typedef struct _AuExten {	/* private to extension mechanism */
	struct _AuExten *next;	/* next in list */
	AuExtCodes codes;	/* public information, all extension told */
	int (*close_server)(	/* routine to call when connection closed */
	    AuServer *,		/* server */
	    AuExtCodes *	/* extensioncodes */
	);
	AuBool (*error)(	/* who to call when an error occurs */
	    AuServer *,		/* server */
	    auError *,		/* errorpacket */
	    AuExtCodes *,	/* extensioncodes */
	    AuBool *		/* returncode */
	);
        char *(*error_string)(  /* routine to supply error string */
	    AuServer *,		/* server */
	    int,		/* errorcode */
	    AuExtCodes *,	/* extensioncodes */
	    char *,		/* buffer */
	    int			/* bufsiz */
	);
	char *name;		/* name of this extension */
	void (*error_values)( /* routine to supply error values */
	    AuServer *,		/* server */
	    AuErrorEvent *,	/* event */
	    FILE *		/* where to print */
	);
} _AuExtension;


#define _AuAddToLinkedList(head, item)					       \
{									       \
    (item)->prev = NULL;						       \
    (item)->next = head;						       \
    if (head)								       \
	(head)->prev = item;						       \
    head = item;							       \
}

#define _AuRemoveFromLinkedList(head, item)				       \
{									       \
    if ((item)->next)							       \
	(item)->next->prev = (item)->prev;				       \
									       \
    if ((item)->prev)							       \
	(item)->prev->next = (item)->next;				       \
    else								       \
	head = (item)->next;						       \
}

/* extension hooks */

_AUFUNCPROTOBEGIN

/**********************/
/* Internal functions */
/**********************/

extern int _AuDefaultError(
    AuServer *,
    AuErrorEvent *
);

extern void
_AuDefaultIOError (
    AuServer *
);

extern AuBool _AuWireToEvent(
    AuServer *,	/* server */
    AuEvent *,	/* pointer to where event should be reformatted */
    auEvent *	/* wire protocol event */
);

extern AuBool _AuUnknownWireEvent(
    AuServer *,	/* server */
    AuEvent *,	/* pointer to where event should be reformatted */
    auEvent *	/* wire protocol event */
);

extern AuStatus _AuUnknownNativeEvent(
    AuServer *,	/* server */
    AuEvent *,	/* pointer to where event should be reformatted */
    auEvent *	/* wire protocol event */
);

extern AuID _AuAllocID (
    AuServer *		/* server */
);

#ifdef _AuDataRoutineIsProcedure
extern void _AuData(
    AuServer *,		/* server */
    char *,		/* data */
    AuInt32		/* len */
);
#endif

extern int _AuError (			/* prepare to upcall user handler */
    AuServer *,		/* server */
    auError *		/* reply */
);
extern int _AuIOError (			/* prepare to upcall user handler */
    AuServer *		/* server */
);

extern void _AuEatData (		/* swallow data from server */
    AuServer *,		/* server */
    AuUint32	/* nbytes */
);

extern char *_AuAllocScratch (		/* fast memory allocator */
    AuServer *,		/* server */
    AuUint32	/* nbytes */
);

extern AuUint32 _AuSetLastRequestRead(	/* update aud->last_request_read */
    AuServer *,		/* server */
    auGenericReply *	/* reply */
);

extern int _AuGetHostname (		/* get name of this machine */
    char *,		/* buf */
    int			/* len */
);

extern AuBool _AuAsyncErrorHandler (	/* internal error handler */
    AuServer *,		/* server */
    auReply *,		/* reply */
    char *,		/* buf */
    int,		/* len */
    AuPointer		/* data */
);

extern void _AuDoDeqAsyncHandler (
    AuServer *,		/* server */
    _AuAsyncHandler *	/* handler */
);

extern char *_AuGetAsyncReply (		/* get async reply */
    AuServer *,		/* server */
    char *,		/* replbuf */
    auReply *,		/* rep */
    char *,		/* buf */
    int,		/* len */
    int,		/* extra */
    AuBool		/* discard */
);

extern AuBool _AuReply (
    AuServer *,		/* server */
    auReply *,		/* reply */
    int,		/* extra */
    AuBool,		/* discard */
    AuStatus *		/* ret_status */
);

extern AuBool _AuForceRoundTrip (
    AuServer *,		/* server */
    int,		/* error_code */
    int,		/* major */
    int,		/* minor */
    AuStatus *		/* ret_status */
);

#define _AuIfRoundTrip(aud,rsp) \
    (rsp ? _AuForceRoundTrip (aud, 0, 0, 0, (rsp)) : AuTrue)

extern int _AuPrintDefaultError (
    AuServer *,		/* server */
    AuErrorEvent *,	/* event */
    FILE *		/* fp */
);

extern void _AuFreeQ (
    AuServer *		/* server */
);

extern void _AuDoSyncHandle (
    AuServer *		/* server */
);

extern void
_AuAddToBucketCache(
                    AuServer *,		/* server */
                    AuBucketAttributes *	/* bucket attributes */
);

extern AuBucketAttributes *
_AuLookupBucketInCache(
			 AuServer *,		/* server */
			 AuBucketID		/* bucket */
);

extern void
_AuRemoveFromBucketCache(
			 AuServer *,		/* server */
			 AuBucketID		/* bucket */
);

extern void
_AuFreeBucketCache(
			 AuServer *		/* server */
);

extern void
_AuRead(
        AuServer *,		/* server */
	char *,			/* data */
	AuInt32			/* size */
);

void
_AuReadPad(
        AuServer *,		/* server */
	char *,			/* data */
	AuInt32			/* size */
);

extern void
_AuSend(
        AuServer *,		/* server */
	char *,			/* data */
	AuInt32			/* size */
);

extern void
_AuFreeExtData(
    AuExtData *			/* extension */
);

extern int
_AuDisconnectServer(
    int				/* server */
);

void
_AuFreeServerStructure(
    AuServer  *			/* aud */
);

void
_AuFlush(
    AuServer  *			/* aud */
);

int
_AuEventsQueued(
    AuServer  *, 		/* aud */
    int				/* mode */
);

void
_AuReadEvents(
    AuServer  * 		/* aud */
);

int
_AuConnectServer(
    const char *, 		/* server_name */
    char **, 			/* fullnamep */
    int *, 			/* svrnump */
    char **, 			/* auth_namep */
    int *, 			/* auth_namelenp */
    char **, 			/* auth_datap */
    int * 			/* auth_datalenp */
);

/********************/
/* Public functions */
/********************/

extern int (*AuESetCloseServer(
    AuServer*		/* server */,
    int			/* extension */,
    int (*) (
	      AuServer*			/* server */,
              AuExtCodes*		/* codes */
            )		/* proc */    
))(
    AuServer*, AuExtCodes*
);

extern int (*AuESetError(
    AuServer*		/* server */,
    int			/* extension */,
    int (*) (
	      AuServer*			/* server */,
              auError*			/* err */,
              AuExtCodes*		/* codes */,
              int*			/* ret_code */
            )		/* proc */    
))(
    AuServer*, auError*, AuExtCodes*, int*
);

extern char* (*AuESetErrorString(
    AuServer*		/* server */,
    int			/* extension */,
    char* (*) (
	        AuServer*		/* server */,
                int			/* code */,
                AuExtCodes*		/* codes */,
                char*			/* buffer */,
                int			/* nbytes */
              )		/* proc */	       
))(
    AuServer*, int, AuExtCodes*, char*, int
);

extern void (*AuESetPrintErrorValues (
    AuServer*		/* server */,
    int			/* extension */,
    void (*)(
	      AuServer*			/* server */,
	      AuErrorEvent*		/* ev */,
	      void*			/* fp */
	     )		/* proc */
))(
    AuServer*, AuErrorEvent*, void*
);

extern int (*AuESetWireToEvent(
    AuServer*		/* server */,
    int			/* event_number */,
    AuBool (*) (
	       AuServer*		/* server */,
               AuEvent*			/* re */,
               auEvent*			/* event */
             )		/* proc */    
))(
    AuServer*, AuEvent*, auEvent*
);

extern AuStatus (*AuESetEventToWire(
    AuServer*		/* server */,
    int			/* event_number */,
    int (*) (
	      AuServer*			/* server */,
              AuEvent*			/* re */,
              auEvent*			/* event */
            )		/* proc */   
))(
    AuServer*, AuEvent*, auEvent*
);

extern AuStatus (*AuESetWireToError(
    AuServer*		/* server */,
    int			/* error_number */,
    AuBool (*) (
	       AuServer*		/* server */,
	       AuErrorEvent*		/* he */,
	       auError*			/* we */
            )		/* proc */   
))(
    AuServer*, AuErrorEvent*, auError*
);

extern AuSyncHandlerRec *
AuRegisterSyncHandler(
		      AuServer *,		/* server */
		      AuSyncHandlerCallback,	/* callback */
		      AuPointer			/* data */
);

extern void
AuUnregisterSyncHandler(
			AuServer *,		/* server */
			AuSyncHandlerRec *	/* handler */
);

extern AuEventEnqHandlerRec *
AuRegisterEventEnqHandler
(
		      AuServer *,		/* server */
 		      int,			/* who */
		      AuEventEnqHandlerCallback,/* callback */
		      AuPointer			/* data */
);

extern void
AuUnregisterEventEnqHandler(
			AuServer *,		/* server */
			AuEventEnqHandlerRec *	/* handler */
);

_AUFUNCPROTOEND

#endif /* _NCD_ALIBINT_H_ */
