//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// The following only applies to changes made to this file as part of YugaByte development.
//
// Portions Copyright (c) YugaByte, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// File names used by DB code

#pragma once

#include <stdint.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "yb/rocksdb/options.h"
#include "yb/rocksdb/status.h"
#include "yb/rocksdb/transaction_log.h"

#include "yb/util/slice.h"

namespace rocksdb {

class Env;
class Directory;
class WritableFileWriter;

enum FileType {
  kLogFile,
  kDBLockFile,
  kTableFile,
  kTableSBlockFile,
  kDescriptorFile,
  kCurrentFile,
  kTempFile,
  kInfoLogFile,  // Either the current one, or an old one
  kMetaDatabase,
  kIdentityFile,
  kOptionsFile
};

// Return the name of the log file with the specified number
// in the db named by "dbname".  The result will be prefixed with
// "dbname".
extern std::string LogFileName(const std::string& dbname, uint64_t number);

static const char ARCHIVAL_DIR[] = "archive";

extern std::string ArchivalDirectory(const std::string& dbname);

//  Return the name of the archived log file with the specified number
//  in the db named by "dbname". The result will be prefixed with "dbname".
extern std::string ArchivedLogFileName(const std::string& dbname,
                                       uint64_t num);

extern std::string MakeTableFileName(const std::string& name, uint64_t number);

// Return the name of sstable with LevelDB suffix
// created from RocksDB sstable suffixed name
extern std::string Rocks2LevelTableFileName(const std::string& fullname);

// the reverse function of MakeTableFileName
// TODO(yhchiang): could merge this function with ParseFileName()
extern uint64_t TableFileNameToNumber(const std::string& name);

// Return the name of the sstable with the specified number
// in the db named by "dbname".  The result will be prefixed with
// "dbname".
extern std::string TableFileName(const std::vector<DbPath>& db_paths,
                                 uint64_t number, uint32_t path_id);

// Return data file name of the sstable for specific base file name.
extern std::string TableBaseToDataFileName(const std::string& base_fname);

// Sufficient buffer size for FormatFileNumber.
const size_t kFormatFileNumberBufSize = 38;

extern void FormatFileNumber(uint64_t number, uint32_t path_id, char* out_buf,
                             size_t out_buf_size);

// Return the name of the descriptor file for the db named by
// "dbname" and the specified incarnation number.  The result will be
// prefixed with "dbname".
extern std::string DescriptorFileName(const std::string& dbname,
                                      uint64_t number);

// Return the name of the current file.  This file contains the name
// of the current manifest file.  The result will be prefixed with
// "dbname".
extern std::string CurrentFileName(const std::string& dbname);

// Return the name of the lock file for the db named by
// "dbname".  The result will be prefixed with "dbname".
extern std::string LockFileName(const std::string& dbname);

// Return the name of a temporary file owned by the db named "dbname".
// The result will be prefixed with "dbname".
extern std::string TempFileName(const std::string& dbname, uint64_t number);

// A helper structure for prefix of info log names.
struct InfoLogPrefix {
  char buf[260];
  Slice prefix;
  // Prefix with DB absolute path encoded
  explicit InfoLogPrefix(bool has_log_dir, const std::string& db_absolute_path);
  // Default Prefix
  InfoLogPrefix();
};

// Return the name of the info log file for "dbname".
extern std::string InfoLogFileName(const std::string& dbname,
                                   const std::string& db_path = "",
                                   const std::string& log_dir = "");

// Return the name of the old info log file for "dbname".
extern std::string OldInfoLogFileName(const std::string& dbname, uint64_t ts,
                                      const std::string& db_path = "",
                                      const std::string& log_dir = "");

static const char kOptionsFileNamePrefix[] = "OPTIONS-";
static const char kTempFileNameSuffix[] = "dbtmp";

// Return a options file name given the "dbname" and file number.
// Format:  OPTIONS-[number].dbtmp
extern std::string OptionsFileName(const std::string& dbname,
                                   uint64_t file_num);

// Return a temp options file name given the "dbname" and file number.
// Format:  OPTIONS-[number]
extern std::string TempOptionsFileName(const std::string& dbname,
                                       uint64_t file_num);

// Return the name to use for a metadatabase. The result will be prefixed with
// "dbname".
extern std::string MetaDatabaseName(const std::string& dbname,
                                    uint64_t number);

// Return the name of the Identity file which stores a unique number for the db
// that will get regenerated if the db loses all its data and is recreated fresh
// either from a backup-image or empty
extern std::string IdentityFileName(const std::string& dbname);

// If filename is a rocksdb file, store the type of the file in *type.
// The number encoded in the filename is stored in *number.  If the
// filename was successfully parsed, returns true.  Else return false.
// info_log_name_prefix is the path of info logs.
extern bool ParseFileName(const std::string& filename, uint64_t* number,
                          const Slice& info_log_name_prefix, FileType* type,
                          WalFileType* log_type = nullptr);
// Same as previous function, but skip info log files.
extern bool ParseFileName(const std::string& filename, uint64_t* number,
                          FileType* type, WalFileType* log_type = nullptr);

// Make the CURRENT file point to the descriptor file with the
// specified number.
extern Status SetCurrentFile(Env* env, const std::string& dbname,
                             uint64_t descriptor_number,
                             Directory* directory_to_fsync,
                             bool disable_data_sync);

// Make the IDENTITY file for the db
extern Status SetIdentityFile(Env* env, const std::string& dbname);

// Sync manifest file `file`.
extern Status SyncManifest(Env* env, const DBOptions* db_options,
                           WritableFileWriter* file);

}  // namespace rocksdb
