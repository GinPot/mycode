/* atomic.c : perform atomic initialization
 *
 * ====================================================================
 * Copyright (c) 2000-2005 CollabNet.  All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at http://subversion.tigris.org/license-1.html.
 * If newer versions of this license are posted there, you may use a
 * newer version instead, at your option.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://subversion.tigris.org/.
 * ====================================================================
 */

#include <apr_time.h>
#include "private/svn_atomic.h"

/* Magic values for atomic initialization */
#define SVN_ATOMIC_UNINITIALIZED 0
#define SVN_ATOMIC_START_INIT    1
#define SVN_ATOMIC_INIT_FAILED   2
#define SVN_ATOMIC_INITIALIZED   3

svn_error_t*
svn_atomic__init_once(volatile svn_atomic_t *global_status,
                      svn_error_t *(*init_func)(apr_pool_t*), apr_pool_t* pool)
{
  /* We have to call init_func exactly once.  Because APR
     doesn't have statically-initialized mutexes, we implement a poor
     man's spinlock using svn_atomic_cas. */
  svn_atomic_t status = svn_atomic_cas(global_status,
                                       SVN_ATOMIC_START_INIT,
                                       SVN_ATOMIC_UNINITIALIZED);

  if (status == SVN_ATOMIC_UNINITIALIZED)
    {
      svn_error_t *err = init_func(pool);
      if (err)
        {
#if APR_HAS_THREADS
          /* Tell other threads that the initialization failed. */
          svn_atomic_cas(global_status,
                         SVN_ATOMIC_INIT_FAILED,
                         SVN_ATOMIC_START_INIT);
#endif
          return err;
        }
      svn_atomic_cas(global_status,
                     SVN_ATOMIC_INITIALIZED,
                     SVN_ATOMIC_START_INIT);
    }
#if APR_HAS_THREADS
  /* Wait for whichever thread is performing initialization to finish. */
  /* XXX FIXME: Should we have a maximum wait here, like we have in
                the Windows file IO spinner? */
  else while (status != SVN_ATOMIC_INITIALIZED)
    {
      if (status == SVN_ATOMIC_INIT_FAILED)
        return svn_error_create(SVN_ERR_FS_GENERAL, NULL,
                                "Couldn't perform atomic initialization");

      apr_sleep(APR_USEC_PER_SEC / 1000);
      status = svn_atomic_cas(global_status,
                              SVN_ATOMIC_UNINITIALIZED,
                              SVN_ATOMIC_UNINITIALIZED);
    }
#endif /* APR_HAS_THREADS */

  return SVN_NO_ERROR;
}
