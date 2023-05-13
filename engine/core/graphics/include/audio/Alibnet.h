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
 * $Id: Alibnet.h 270 2007-11-22 23:53:23Z steve_mcintyre $
 * $NCDId: @(#)Alibnet.h,v 1.8 1995/12/06 01:16:57 greg Exp $
 */

/* Portions derived from */
/* $XConsortium: Xlibnet.h,v 1.19 92/10/21 10:22:55 rws Exp $ */

/*
Copyright 1991 Massachusetts Institute of Technology

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
 * Xlibnet.h - Xlib networking include files for UNIX Systems.
 */

#ifndef _NCD_ALIBNET_H_
#define _NCD_ALIBNET_H_

#ifndef AU_UNIX_PATH
#ifdef hpux
#define AU_UNIX_PATH "/usr/spool/sockets/ncdaudio/"
#define OLD_UNIX_PATH "/tmp/.ncdaudio-unix/X"
#else
#define AU_UNIX_PATH "/tmp/.ncdaudio-unix/X"
#endif
#endif /* AU_UNIX_PATH */

#ifdef WIN32
#include <winsock.h>

#define ECONNREFUSED            WSAECONNREFUSED
#define sleep(x)		Sleep(x * 1000)
#define TCPCONN
#define BytesReadable(fd, ptr) \
    ioctlsocket (fd, FIONREAD, (unsigned long *) (ptr))
#endif /* WIN32 */

#ifdef STREAMSCONN
#ifdef SYSV
/*
 * UNIX System V Release 3.2
 */
#define BytesReadable(fd,ptr) (_AuBytesReadable ((fd), (ptr)))
#define MALLOC_0_RETURNS_NULL
#include <sys/ioctl.h>

#endif /* SYSV */
#ifdef SVR4
/*
 * TLI (Streams-based) networking
 */
#define BytesReadable(fd,ptr) (_AuBytesReadable ((fd), (ptr)))
#include <sys/uio.h>		/* define struct iovec */

#endif /* SVR4 */
#else /* not STREAMSCONN */
#ifndef WIN32
/*
 * socket-based systems
 */
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/uio.h>	/* needed for AlibInt.c */
#ifdef SVR4
#include <sys/filio.h>
#endif

#if defined(SYSV386) && defined(SYSV)
#include <net/errno.h>
#include <sys/stropts.h>
#define BytesReadable(fd,ptr) ioctl((fd), I_NREAD, (ptr))
#else
#define BytesReadable(fd, ptr) ioctl ((fd), FIONREAD, (ptr))
#endif
#endif /* !WIN32 */

#endif /* STREAMSCONN else */

/*
 * If your BytesReadable correctly detects broken connections, then
 * you should NOT define AUCONN_CHECK_FREQ.
 */
#define AUCONN_CHECK_FREQ 256

#ifndef AU_NOT_POSIX
#ifdef _POSIX_SOURCE
#include <limits.h>
#else
#define _POSIX_SOURCE
#include <limits.h>
#undef _POSIX_SOURCE
#endif
#endif
#ifndef OPEN_MAX
#ifdef SVR4
#define OPEN_MAX 256
#else
#ifndef WIN32
#include <sys/param.h>
#else /* WIN32 */
#include <limits.h>
#endif /* WIN32 */
#ifndef OPEN_MAX
#ifdef NOFILE
#define OPEN_MAX NOFILE
#else
#if defined(_POSIX_OPEN_MAX) && !defined(SCO)
#define OPEN_MAX _POSIX_OPEN_MAX
#else
#define OPEN_MAX NOFILES_MAX
#endif
#endif
#endif
#endif
#endif

#if OPEN_MAX > 256
#undef OPEN_MAX
#define OPEN_MAX 256
#endif

#define MSKCNT ((OPEN_MAX + 31) / 32)

#if (MSKCNT==1)
#define BITMASK(i) (1 << (i))
#define MASKIDX(i) 0
#endif
#if (MSKCNT>1)
#define BITMASK(i) (1 << ((i) & 31))
#define MASKIDX(i) ((i) >> 5)
#endif

#define MASKWORD(buf, i) buf[MASKIDX(i)]
#define BITSET(buf, i) MASKWORD(buf, i) |= BITMASK(i)
#define BITCLEAR(buf, i) MASKWORD(buf, i) &= ~BITMASK(i)
#define GETBIT(buf, i) (MASKWORD(buf, i) & BITMASK(i))

#if (MSKCNT==1)
#define COPYBITS(src, dst) dst[0] = src[0]
#define CLEARBITS(buf) buf[0] = 0
#define MASKANDSETBITS(dst, b1, b2) dst[0] = (b1[0] & b2[0])
#define ORBITS(dst, b1, b2) dst[0] = (b1[0] | b2[0])
#define UNSETBITS(dst, b1) (dst[0] &= ~b1[0])
#define _AuANYSET(src) (src[0])
#endif
#if (MSKCNT==2)
#define COPYBITS(src, dst) { dst[0] = src[0]; dst[1] = src[1]; }
#define CLEARBITS(buf) { buf[0] = 0; buf[1] = 0; }
#define MASKANDSETBITS(dst, b1, b2)  {\
		      dst[0] = (b1[0] & b2[0]);\
		      dst[1] = (b1[1] & b2[1]); }
#define ORBITS(dst, b1, b2)  {\
		      dst[0] = (b1[0] | b2[0]);\
		      dst[1] = (b1[1] | b2[1]); }
#define UNSETBITS(dst, b1) {\
                      dst[0] &= ~b1[0]; \
                      dst[1] &= ~b1[1]; }
#define _AuANYSET(src) (src[0] || src[1])
#endif
#if (MSKCNT==3)
#define COPYBITS(src, dst) { dst[0] = src[0]; dst[1] = src[1]; \
			     dst[2] = src[2]; }
#define CLEARBITS(buf) { buf[0] = 0; buf[1] = 0; buf[2] = 0; }
#define MASKANDSETBITS(dst, b1, b2)  {\
		      dst[0] = (b1[0] & b2[0]);\
		      dst[1] = (b1[1] & b2[1]);\
		      dst[2] = (b1[2] & b2[2]); }
#define ORBITS(dst, b1, b2)  {\
		      dst[0] = (b1[0] | b2[0]);\
		      dst[1] = (b1[1] | b2[1]);\
		      dst[2] = (b1[2] | b2[2]); }
#define UNSETBITS(dst, b1) {\
                      dst[0] &= ~b1[0]; \
                      dst[1] &= ~b1[1]; \
                      dst[2] &= ~b1[2]; }
#define _AuANYSET(src) (src[0] || src[1] || src[2])
#endif
#if (MSKCNT==4)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]; \
			   dst[2] = src[2]; dst[3] = src[3]
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
                      dst[0] = (b1[0] & b2[0]);\
                      dst[1] = (b1[1] & b2[1]);\
                      dst[2] = (b1[2] & b2[2]);\
                      dst[3] = (b1[3] & b2[3])
#define ORBITS(dst, b1, b2)  \
                      dst[0] = (b1[0] | b2[0]);\
                      dst[1] = (b1[1] | b2[1]);\
                      dst[2] = (b1[2] | b2[2]);\
                      dst[3] = (b1[3] | b2[3])
#define UNSETBITS(dst, b1) \
                      dst[0] &= ~b1[0]; \
                      dst[1] &= ~b1[1]; \
                      dst[2] &= ~b1[2]; \
                      dst[3] &= ~b1[3]
#define _AuANYSET(src) (src[0] || src[1] || src[2] || src[3])
#endif

#if (MSKCNT>4)
#define COPYBITS(src, dst) bcopy((char *) src, (char *) dst,\
				 MSKCNT*sizeof(AuInt32))
#define CLEARBITS(buf) bzero((char *) buf, MSKCNT*sizeof(AuInt32))
#define MASKANDSETBITS(dst, b1, b2)  \
		      { int cri;			\
			for (cri=MSKCNT; --cri>=0; )	\
		          dst[cri] = (b1[cri] & b2[cri]); }
#define ORBITS(dst, b1, b2)  \
		      { int cri;			\
		      for (cri=MSKCNT; --cri>=0; )	\
		          dst[cri] = (b1[cri] | b2[cri]); }
#define UNSETBITS(dst, b1) \
		      { int cri;			\
		      for (cri=MSKCNT; --cri>=0; )	\
		          dst[cri] &= ~b1[cri];  }
/*
 * If MSKCNT>4, then _AuANYSET is a routine defined in AlibInt.c.
 *
 * #define _AuANYSET(src) (src[0] || src[1] || src[2] || src[3] || src[4] ...)
 */
#endif

/*
 *	ReadvFromServer and WritevToServer use struct iovec, normally found
 *	in Berkeley systems in <sys/uio.h>.  See the readv(2) and writev(2)
 *	manual pages for details.
 *
 *	struct iovec {
 *		caddr_t iov_base;
 *		int iov_len;
 *	};
 */
#if defined(USG) && !defined(CRAY) && !defined(umips) && !defined(MOTOROLA) && !defined(uniosu) && !defined(_UIO_) && !defined(USL)
struct iovec {
    caddr_t iov_base;
    int iov_len;
};
#endif /* USG */


#if defined(STREAMSCONN) 
#include "Astreams.h"

extern char _AusTypeOfStream[];
extern Austream _AusStream[];

#define ReadFromServer(dpy, data, size) \
	(*_AusStream[_AusTypeOfStream[dpy]].ReadFromStream)((dpy), (data), (size), \
						     BUFFERING)
#define WriteToServer(dpy, bufind, size) \
	(*_AusStream[_AusTypeOfStream[dpy]].WriteToStream)((dpy), (bufind), (size))

#else /* else not STREAMSCONN */

/*
 * bsd can read from sockets directly
 */
#if !defined(WIN32) 
#define ReadFromServer(dpy, data, size) read((dpy), (data), (size))
#define WriteToServer(dpy, bufind, size) write((dpy), (bufind), (size))
#else /* WIN32 */
#define ReadFromServer(dpy, data, size) recv((dpy), (data), (size), 0)
#define WriteToServer(dpy, bufind, size) send((dpy), (bufind), (size), 0)
#endif /* WIN32 */

#endif /* STREAMSCONN */

#ifdef ISC
#ifndef USL_COMPAT
#if !defined(USG) || defined(MOTOROLA) || defined(uniosu)
#if !(defined(SYSV) && defined(SYSV386))
#define _AuReadV readv
#endif
#define _AuWriteV writev
#endif
#endif /* !USL_COMPAT */
#endif /* ISC */

#define ReadvFromServer(dpy, iov, iovcnt) _AuReadV((dpy), (iov), (iovcnt))
#define WritevToServer(dpy, iov, iovcnt) _AuWriteV((dpy), (iov), (iovcnt))

_AUFUNCPROTOBEGIN

#ifndef _AuANYSET
extern int
_AuANYSET(
    AuInt32 * 			/* src */
);
#endif /* !_AuANYSET */

int _AuReadV (
    int,  			/* fd */
    struct iovec *, 		/* iov */
    int 			/* iovcnt */
);

int _AuWriteV (
    int,  			/* fd */
    struct iovec *, 		/* iov */
    int 			/* iovcnt */
);

_AUFUNCPROTOEND

#ifdef WIN32
typedef unsigned char * caddr_t;

struct iovec
{
    caddr_t   iov_base;
    int  iov_len;
};

#endif /* WIN32 */

#endif /* _NCD_ALIBNET_H_ */
