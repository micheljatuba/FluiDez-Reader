#include "SleepImageConverter.h"

#include <FsHelpers.h>
#include <HalStorage.h>
#include <JpegToBmpConverter.h>
#include <Logging.h>
#include <PngToBmpConverter.h>

#include <cstdint>
#include <cstdio>

namespace SleepImageConverter {
namespace {

constexpr char CACHE_DIR[] = "/.crosspoint/sleep-converted";

// The key covers the source path, its size and the tone mapping, so replacing
// a photo under the same name or changing the sleep filter converts again.
uint32_t cacheKey(const std::string& path, const uint64_t size, const bool imageLevels) {
  uint32_t hash = 2166136261u;
  const auto mix = [&hash](const uint8_t byte) {
    hash ^= byte;
    hash *= 16777619u;
  };
  for (const char c : path) mix(static_cast<uint8_t>(c));
  for (int shift = 0; shift < 64; shift += 8) mix(static_cast<uint8_t>(size >> shift));
  mix(imageLevels ? 1 : 0);
  return hash;
}

}  // namespace

bool isConvertible(const std::string& path) {
  return FsHelpers::hasJpgExtension(path) || FsHelpers::hasPngExtension(path);
}

bool resolveRenderableBmp(const std::string& imagePath, const bool imageLevels, std::string& bmpPath) {
  if (!isConvertible(imagePath)) {
    bmpPath = imagePath;
    return true;
  }

  FsFile source;
  if (!Storage.openFileForRead("SLPCONV", imagePath, source)) return false;

  char cachePath[64];
  snprintf(cachePath, sizeof(cachePath), "%s/%08lx.bmp", CACHE_DIR,
           static_cast<unsigned long>(cacheKey(imagePath, source.fileSize64(), imageLevels)));
  if (Storage.exists(cachePath)) {
    source.close();
    bmpPath = cachePath;
    return true;
  }

  if (!Storage.ensureDirectoryExists(CACHE_DIR)) {
    source.close();
    LOG_ERR("SLPCONV", "Cannot create %s", CACHE_DIR);
    return false;
  }

  // Write beside the final name and rename, so a power cut mid-decode never
  // leaves a truncated BMP that later sleeps would trust.
  char tempPath[68];
  snprintf(tempPath, sizeof(tempPath), "%s.tmp", cachePath);
  FsFile output;
  if (!Storage.openFileForWrite("SLPCONV", tempPath, output)) {
    source.close();
    return false;
  }

  LOG_INF("SLPCONV", "Converting sleep photo %s", imagePath.c_str());
  const bool ok = FsHelpers::hasJpgExtension(imagePath)
                      ? JpegToBmpConverter::jpegFileToBmpStream(source, output, true, imageLevels)
                      : PngToBmpConverter::pngFileToBmpStream(source, output, true, imageLevels);
  source.close();
  output.close();

  if (!ok || !Storage.rename(tempPath, cachePath)) {
    LOG_ERR("SLPCONV", "Sleep photo conversion failed: %s", imagePath.c_str());
    Storage.remove(tempPath);
    return false;
  }
  bmpPath = cachePath;
  return true;
}

}  // namespace SleepImageConverter
