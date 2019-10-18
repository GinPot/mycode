/*
 * svn_fs_private.h: Private declarations for the filesystem layer to
 * be consumed by libsvn_fs* and non-libsvn_fs* modules.
 *
 * ====================================================================
 * Copyright (c) 2007 CollabNet.  All rights reserved.
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

#ifndef SVN_FS_PRIVATE_H
#define SVN_FS_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The maximum length of a transaction name.  The Berkeley DB backend
   generates transaction names from a sequence expressed as a base 36
   number with a maximum of MAX_KEY_SIZE (currently 200) bytes.  The
   FSFS backend generates transaction names of the form
   <rev>-<base 36-number> where the base 36 number is a sequence value
   with a maximum length of MAX_KEY_SIZE bytes.  The maximum length is
   212, but use 220 just to have some extra space:
     10   -> 32 bit revision number
     1    -> '-'
     200  -> 200 digit base 36 number
     1    -> '\0'
 */
#define SVN_FS__TXN_MAX_LEN 220

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SVN_FS_PRIVATE_H */
