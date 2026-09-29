#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

// In-memory SD card with failure injection for persistence tests.
struct MemoryFs {
  std::map<std::string, std::vector<uint8_t>> files;
  bool failRename = false;
  std::string failRemovePath;
};

class FsFile {
 public:
  explicit operator bool() const { return fs_ != nullptr; }

  int read(void* buffer, const size_t size) {
    if (!fs_) return -1;
    const auto& data = fs_->files[path_];
    const size_t count = pos_ < data.size() ? std::min(size, data.size() - pos_) : 0;
    std::memcpy(buffer, data.data() + pos_, count);
    pos_ += count;
    return static_cast<int>(count);
  }

  size_t write(const void* data, const size_t size) {
    if (!fs_) return 0;
    const auto* bytes = static_cast<const uint8_t*>(data);
    auto& file = fs_->files[path_];
    file.insert(file.end(), bytes, bytes + size);
    return size;
  }

  void flush() {}
  bool sync() { return fs_ != nullptr; }
  bool close() {
    const bool wasOpen = fs_ != nullptr;
    fs_ = nullptr;
    return wasOpen;
  }

 private:
  friend class HalStorage;
  MemoryFs* fs_ = nullptr;
  std::string path_;
  size_t pos_ = 0;
};

class HalStorage {
 public:
  MemoryFs fs;

  bool exists(const char* path) const { return fs.files.count(path) != 0; }

  bool remove(const char* path) {
    if (fs.failRemovePath == path) return false;
    return fs.files.erase(path) != 0;
  }

  bool rename(const char* from, const char* to) {
    if (fs.failRename || fs.files.count(from) == 0 || fs.files.count(to) != 0) return false;
    fs.files[to] = std::move(fs.files[from]);
    fs.files.erase(from);
    return true;
  }

  bool openFileForRead(const char*, const std::string& path, FsFile& file) {
    if (fs.files.count(path) == 0) return false;
    file.fs_ = &fs;
    file.path_ = path;
    file.pos_ = 0;
    return true;
  }

  bool openFileForWrite(const char*, const std::string& path, FsFile& file) {
    fs.files[path].clear();
    file.fs_ = &fs;
    file.path_ = path;
    file.pos_ = 0;
    return true;
  }
};

extern HalStorage Storage;
