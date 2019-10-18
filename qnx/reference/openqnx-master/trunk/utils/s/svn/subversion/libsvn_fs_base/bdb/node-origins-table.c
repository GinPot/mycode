/* node-origins-table.c : operations on the `node-origins' table
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

#include "bdb_compat.h"
#include "../fs.h"
#include "../err.h"
#include "../id.h"
#include "dbt.h"
#include "../trail.h"
#include "bdb-err.h"
#include "../../libsvn_fs/fs-loader.h"
#include "node-origins-table.h"

#include "svn_private_config.h"


int svn_fs_bdb__open_node_origins_table(DB **node_origins_p,
                                        DB_ENV *env,
                                        svn_boolean_t create)
{
  const u_int32_t open_flags = (create ? (DB_CREATE | DB_EXCL) : 0);
  DB *node_origins;
  int error;

  BDB_ERR(svn_fs_bdb__check_version());
  BDB_ERR(db_create(&node_origins, env, 0));
  error = (node_origins->open)(SVN_BDB_OPEN_PARAMS(node_origins, NULL),
                               "node-origins", 0, DB_BTREE,
                               open_flags, 0666);

  /* Create the node-origins table if it doesn't exist. */
  if (error == ENOENT && (! create))
    {
      BDB_ERR(node_origins->close(node_origins, 0));
      return svn_fs_bdb__open_node_origins_table(node_origins_p, env, TRUE);
    }

  BDB_ERR(error);

  *node_origins_p = node_origins;
  return 0;
}

svn_error_t *svn_fs_bdb__get_node_origin(const svn_fs_id_t **origin_id,
                                         svn_fs_t *fs,
                                         const char *node_id,
                                         trail_t *trail,
                                         apr_pool_t *pool)
{
  base_fs_data_t *bfd = fs->fsap_data;
  DBT key, value;
  int db_err;

  svn_fs_base__trail_debug(trail, "node-origins", "get");
  db_err = bfd->node_origins->get(bfd->node_origins, trail->db_txn,
                                  svn_fs_base__str_to_dbt(&key, node_id),
                                  svn_fs_base__result_dbt(&value), 0);
  svn_fs_base__track_dbt(&value, pool);

  if (db_err == DB_NOTFOUND)
    return svn_fs_base__err_no_such_node_origin(fs, node_id);

  *origin_id = svn_fs_base__id_parse(value.data, value.size, pool);
  return SVN_NO_ERROR;
}

svn_error_t *svn_fs_bdb__set_node_origin(svn_fs_t *fs,
                                         const char *node_id,
                                         const svn_fs_id_t *origin_id,
                                         trail_t *trail,
                                         apr_pool_t *pool)
{
  base_fs_data_t *bfd = fs->fsap_data;
  DBT key, value;
  int db_err;

  /* Create a key from our NODE_ID. */
  svn_fs_base__str_to_dbt(&key, node_id);

  /* Check to see if we already have a mapping for NODE_ID.  If so,
     and the value is the same one we were about to write.  That's
     cool -- just do nothing.  If, however, the value is *different*,
     that's a red flag!  */
  svn_fs_base__trail_debug(trail, "node-origins", "get");
  db_err = bfd->node_origins->get(bfd->node_origins, trail->db_txn,
                                  &key, svn_fs_base__result_dbt(&value), 0);
  svn_fs_base__track_dbt(&value, pool);
  if (db_err != DB_NOTFOUND)
    {
      const svn_string_t *origin_id_str = svn_fs_base__id_unparse(origin_id, pool);
      const svn_string_t *old_origin_id_str =
        svn_string_ncreate(value.data, value.size, pool);

      if (! svn_string_compare(origin_id_str, old_origin_id_str))
        return svn_error_createf
          (SVN_ERR_FS_CORRUPT, NULL,
           _("Node origin for '%s' exists in filesystem '%s' with a different "
             "value (%s) than what we were about to store (%s)"),
           node_id, fs->path, old_origin_id_str->data, origin_id_str->data);
      else
        return SVN_NO_ERROR;
    }

  /* Create a value from our ORIGIN_ID, and add this record to the table. */
  svn_fs_base__id_to_dbt(&value, origin_id, pool);
  svn_fs_base__trail_debug(trail, "node-origins", "put");
  SVN_ERR(BDB_WRAP(fs, _("storing node-origins record"),
                   bfd->node_origins->put(bfd->node_origins, trail->db_txn,
                                          &key, &value, 0)));
  return SVN_NO_ERROR;
}

svn_error_t *svn_fs_bdb__delete_node_origin(svn_fs_t *fs,
                                            const char *node_id,
                                            trail_t *trail,
                                            apr_pool_t *pool)
{
  base_fs_data_t *bfd = fs->fsap_data;
  DBT key;

  svn_fs_base__str_to_dbt(&key, node_id);
  svn_fs_base__trail_debug(trail, "node-origins", "del");
  SVN_ERR(BDB_WRAP(fs, "deleting entry from 'node-origins' table",
                   bfd->node_origins->del(bfd->node_origins,
                                          trail->db_txn, &key, 0)));
  return SVN_NO_ERROR;
}
