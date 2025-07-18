// Copyright (c) 2022 and onwards The McBopomofo Authors.
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "PhraseReplacementMap.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <string>

#include "KeyValueBlobReader.h"

namespace McBopomofo {

bool PhraseReplacementMap::open(const char* path) {
  if (!mmapedFile_.open(path)) {
    return false;
  }

  // MemoryMappedFile self-closes, and so this is fine.
  return load(mmapedFile_.data(), mmapedFile_.length());
}

void PhraseReplacementMap::close() {
  keyValueMap_.clear();
  mmapedFile_.close();
}

bool PhraseReplacementMap::load(const char* data, size_t length) {
  if (data == nullptr || length == 0) {
    return false;
  }
  keyValueMap_.clear();

  KeyValueBlobReader reader(data, length);
  KeyValueBlobReader::KeyValue keyValue;
  while (reader.Next(&keyValue) == KeyValueBlobReader::State::HAS_PAIR) {
    keyValueMap_[keyValue.key] = keyValue.value;
  }
  return true;
}

std::string PhraseReplacementMap::valueForKey(const std::string& key) const {
  auto iter = keyValueMap_.find(key);
  if (iter != keyValueMap_.end()) {
    const std::string_view& v = iter->second;
    return std::string(v);
  }
  return {};
}

}  // namespace McBopomofo
