/*
 * compat.c :  Wrappers and callbacks for compatibility.
 *
 * ====================================================================
 * Copyright (c) 2005 CollabNet.  All rights reserved.
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

#include <apr_pools.h>
#include <apr_strings.h>

#include "svn_types.h"
#include "svn_error.h"
#include "svn_compat.h"


/* Baton for use with svn_compat_wrap_commit_callback */
struct commit_wrapper_baton {
  void *baton;
  svn_commit_callback_t callback;
};

/* This implements svn_commit_callback2_t. */
static svn_error_t *
commit_wrapper_callback(const svn_commit_info_t *commit_info,
                        void *baton, apr_pool_t *pool)
{
  struct commit_wrapper_baton *cwb = baton;

  if (cwb->callback)
    return cwb->callback(commit_info->revision,
                         commit_info->date,
                         commit_info->author,
                         cwb->baton);

  return SVN_NO_ERROR;
}

void
svn_compat_wrap_commit_callback(svn_commit_callback2_t *callback2,
                                void **callback2_baton,
                                svn_commit_callback_t callback,
                                void *callback_baton,
                                apr_pool_t *pool)
{
  struct commit_wrapper_baton *cwb = apr_palloc(pool, sizeof(*cwb));

  /* Set the user provided old format callback in the baton */
  cwb->baton = callback_baton;
  cwb->callback = callback;

  *callback2_baton = cwb;
  *callback2 = commit_wrapper_callback;
}


void
svn_compat_log_revprops_clear(apr_hash_t *revprops)
{
  if (revprops)
    {
      apr_hash_set(revprops, SVN_PROP_REVISION_AUTHOR,
                   APR_HASH_KEY_STRING, NULL);
      apr_hash_set(revprops, SVN_PROP_REVISION_DATE,
                   APR_HASH_KEY_STRING, NULL);
      apr_hash_set(revprops, SVN_PROP_REVISION_LOG,
                   APR_HASH_KEY_STRING, NULL);
    }
}

apr_array_header_t *
svn_compat_log_revprops_in(apr_pool_t *pool)
{
  apr_array_header_t *revprops = apr_array_make(pool, 3, sizeof(char *));

  APR_ARRAY_PUSH(revprops, const char *) = SVN_PROP_REVISION_AUTHOR;
  APR_ARRAY_PUSH(revprops, const char *) = SVN_PROP_REVISION_DATE;
  APR_ARRAY_PUSH(revprops, const char *) = SVN_PROP_REVISION_LOG;

  return revprops;
}

void
svn_compat_log_revprops_out(const char **author, const char **date,
                            const char **message, apr_hash_t *revprops)
{
  svn_string_t *author_s, *date_s,  *message_s;

  *author = *date = *message = NULL;
  if (revprops)
    {
      if ((author_s = apr_hash_get(revprops, SVN_PROP_REVISION_AUTHOR,
                                   APR_HASH_KEY_STRING)))
        *author = author_s->data;
      if ((date_s = apr_hash_get(revprops, SVN_PROP_REVISION_DATE,
                                 APR_HASH_KEY_STRING)))
        *date = date_s->data;
      if ((message_s = apr_hash_get(revprops, SVN_PROP_REVISION_LOG,
                                    APR_HASH_KEY_STRING)))
        *message = message_s->data;
    }
}

/* Baton for use with svn_compat_wrap_log_receiver */
struct log_wrapper_baton {
  void *baton;
  svn_log_message_receiver_t receiver;
};

/* This implements svn_log_entry_receiver_t. */
static svn_error_t *
log_wrapper_callback(void *baton,
                     svn_log_entry_t *log_entry,
                     apr_pool_t *pool)
{
  struct log_wrapper_baton *lwb = baton;

  if (lwb->receiver && SVN_IS_VALID_REVNUM(log_entry->revision))
    {
      const char *author, *date, *message;
      svn_compat_log_revprops_out(&author, &date, &message,
                                  log_entry->revprops);
      return lwb->receiver(lwb->baton,
                           log_entry->changed_paths,
                           log_entry->revision,
                           author, date, message,
                           pool);
    }

  return SVN_NO_ERROR;
}

void
svn_compat_wrap_log_receiver(svn_log_entry_receiver_t *receiver2,
                             void **receiver2_baton,
                             svn_log_message_receiver_t receiver,
                             void *receiver_baton,
                             apr_pool_t *pool)
{
  struct log_wrapper_baton *lwb = apr_palloc(pool, sizeof(*lwb));

  /* Set the user provided old format callback in the baton. */
  lwb->baton = receiver_baton;
  lwb->receiver = receiver;

  *receiver2_baton = lwb;
  *receiver2 = log_wrapper_callback;
}
