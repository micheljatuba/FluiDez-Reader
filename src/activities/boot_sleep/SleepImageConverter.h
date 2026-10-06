#pragma once

#include <string>

// Lets the Custom sleep screen use JPG/PNG files copied straight to the SD
// card. The first use converts the photo once into a screen-sized BMP under
// /.crosspoint/sleep-converted/; later sleeps only stream that BMP, so the
// photo costs the same as a BMP from then on.
namespace SleepImageConverter {

// True for the photo formats this module can convert (.jpg, .jpeg, .png).
bool isConvertible(const std::string& path);

// Resolves imagePath to a BMP the sleep renderer can stream. BMPs pass through
// unchanged; photos are converted on first use (cropped to fill the screen).
// Returns false when the photo cannot be decoded or the cache is not writable.
bool resolveRenderableBmp(const std::string& imagePath, bool imageLevels, std::string& bmpPath);

}  // namespace SleepImageConverter
