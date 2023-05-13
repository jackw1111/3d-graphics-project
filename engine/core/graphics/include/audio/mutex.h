/*
 * mutexes for libaudio
 * 
 * Jon Trulson, 11/25/2001
 *
 * $Id: mutex.h 87 2002-02-24 04:39:08Z jon $
 *
 */

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


#include "NasConfig.h"		/* see if we should try to use mutexes */

#ifndef _MUTEX_H_INCLUDED
#define _MUTEX_H_INCLUDED

/*JET*/
/*#define DEBUG_MUTEX*/

#ifdef DEBUG_MUTEX
# define MDBG_START(x) fprintf(stderr, "%s: %s@%d ...", x, __FILE__, __LINE__);
# define MDBG_END() fprintf(stderr, "done\n");
#else
# define MDBG_START(x)
# define MDBG_END()
#endif

#include <audio/audiolib.h>

#include <stdio.h>
#include <errno.h>

#include <audio/Aproto.h>
#include <audio/Afuncs.h>
#include <audio/Aosdefs.h>

/* NAS_USEMTSAFEAPI is defined in config/NasConfig.h.  If undefined
   thread safety will be disabled */

#if defined(XTHREADS) && defined(NAS_USEMTSAFEAPI)

# include <X11/Xthreads.h>
typedef xmutex_rec _AuMutex;
# ifndef XMUTEX_INITIALIZER

				/*  These systems don't seem to define
				    XMUTEX_INITIALIZER, even though they
				    should */
#  if defined(SVR4) && (defined(USL) || defined(sun))
#   define XMUTEX_INITIALIZER {0}
#  else
#   define XMUTEX_INITIALIZER 0
#  endif
# endif
# define _AU_MUTEX_INITIALIZER    XMUTEX_INITIALIZER
#else
typedef unsigned int _AuMutex;
# define _AU_MUTEX_INITIALIZER    0
#endif

#ifdef _INSTANTIATE_GLOBALS
_AuMutex _serv_mutex = _AU_MUTEX_INITIALIZER;
_AuMutex _init_mutex = _AU_MUTEX_INITIALIZER;
#else
extern _AuMutex _serv_mutex;
extern _AuMutex _init_mutex;
#endif


/* NAS_USEMTSAFEAPI is defined in config/NasConfig.h.  If undefined
   thread safety will be disabled */

#if defined(XTHREADS) && defined(NAS_USEMTSAFEAPI)
#define _AuLockServer()      { \
                               MDBG_START("LOCK   _serv_mutex"); \
                               xmutex_lock(&_serv_mutex); \
                               MDBG_END(); \
                             }
#define _AuUnlockServer()    { \
                               MDBG_START("UNLOCK _serv_mutex"); \
                               xmutex_unlock(&_serv_mutex); \
                               MDBG_END(); \
                             }

#define _AuLockMutex(x)      { \
                               MDBG_START("LOCK   mutex"); \
                               xmutex_lock(&(x)); \
                               MDBG_END(); \
                             }
#define _AuUnlockMutex(x)    { \
                               MDBG_START("UNLOCK  mutex"); \
                               xmutex_unlock(&(x)); \
                               MDBG_END(); \
                             }

#else
#define _AuLockServer()
#define _AuUnlockServer()

#define _AuLockMutex(x)
#define _AuUnlockMutex(x)
#endif



#endif /* _MUTEX_H_INCLUDED */
