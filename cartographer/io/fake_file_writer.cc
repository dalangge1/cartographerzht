/*
 * Copyright 2018 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cartographer/io/fake_file_writer.h"

#include <sstream>
#include "cartographer/common/make_unique.h"

namespace cartographer {
namespace io {

FakeStreamFileWriter::FakeStreamFileWriter(const std::string filename)
    : StreamWriter(common::make_unique<std::ostringstream>(), filename) {}

std::string FakeStreamFileWriter::GetOutput() const {
  return static_cast<std::ostringstream*>(out_.get())->str();
}

}  // namespace io
}  // namespace cartographer
