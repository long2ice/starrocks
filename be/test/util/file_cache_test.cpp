// This file is made available under Elastic License 2.0.
// This file is based on code available under the Apache license here:
//   https://github.com/apache/incubator-doris/blob/master/be/test/util/file_cache_test.cpp

// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "util/file_cache.h"

#include <gtest/gtest.h>

#include "env/env.h"

namespace starrocks {

class FileCacheTest : public testing::Test {
public:
    FileCacheTest() {}

    void SetUp() override {
        _file_cache.reset(new FileCache<RandomAccessFile>("test cache", 10000));
        _file_exist = "file_exist";
        auto file = *Env::Default()->new_writable_file(_file_exist);
        auto st = file->close();
        ASSERT_TRUE(st.ok());
    }

    void TearDown() override {
        _file_cache.reset(nullptr);
        auto st = Env::Default()->delete_file(_file_exist);
        ASSERT_TRUE(st.ok());
    }

private:
    std::unique_ptr<FileCache<RandomAccessFile>> _file_cache;
    std::string _file_exist;
};

TEST_F(FileCacheTest, normal) {
    OpenedFileHandle<RandomAccessFile> file_handle;
    auto found = _file_cache->lookup(_file_exist, &file_handle);
    ASSERT_FALSE(found);
    auto file = *Env::Default()->new_random_access_file(_file_exist);
    RandomAccessFile* tmp_file = file.release();
    _file_cache->insert(_file_exist, tmp_file, &file_handle);
    ASSERT_EQ(tmp_file, file_handle.file());
    OpenedFileHandle<RandomAccessFile> file_handle2;
    found = _file_cache->lookup(_file_exist, &file_handle2);
    ASSERT_EQ(file_handle.file(), file_handle2.file());
}

} // namespace starrocks
