#include <gtest/gtest.h>

#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace {

std::string readFile(const std::filesystem::path& path) {
  std::ifstream source(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(source), std::istreambuf_iterator<char>()};
}

std::string withoutWhitespace(std::string_view text) {
  std::string compact;
  compact.reserve(text.size());
  for (const char c : text) {
    if (!std::isspace(static_cast<unsigned char>(c))) compact += c;
  }
  return compact;
}

bool isUnconditionalTrue(const std::string& signatureAndBody) {
  return signatureAndBody == "()override{returntrue;}" || signatureAndBody == "(){returntrue;}" ||
         signatureAndBody == "()constoverride{returntrue;}";
}

// An unconditional override kept idle input, list, and result screens awake or
// spinning at full CPU speed until the user returned.
TEST(ActivityPowerPolicyGuard, IdleScreensDoNotUnconditionallyBlockSleepOrLoopDelay) {
  constexpr std::array<std::string_view, 2> policies = {"preventAutoSleep", "skipLoopDelay"};
  std::vector<std::string> offenders;
  size_t scannedFiles = 0;

  for (const auto& entry : std::filesystem::recursive_directory_iterator(ACTIVITIES_SOURCE_DIR)) {
    if (!entry.is_regular_file()) continue;
    const auto extension = entry.path().extension();
    if (extension != ".h" && extension != ".cpp") continue;

    ++scannedFiles;
    const std::string source = readFile(entry.path());
    for (const std::string_view policy : policies) {
      for (size_t pos = source.find(policy); pos != std::string::npos; pos = source.find(policy, pos + 1)) {
        const size_t start = pos + policy.size();
        const size_t close = source.find('}', start);
        if (close == std::string::npos) break;
        if (isUnconditionalTrue(withoutWhitespace(std::string_view(source).substr(start, close + 1 - start)))) {
          offenders.push_back(entry.path().filename().string() + ": " + std::string(policy));
        }
      }
    }
  }

  ASSERT_GT(scannedFiles, 0U);
  std::string report;
  for (const auto& offender : offenders) report += "\n  " + offender;
  EXPECT_TRUE(offenders.empty()) << "Tie these policies to bounded work states:" << report;
}

}  // namespace
