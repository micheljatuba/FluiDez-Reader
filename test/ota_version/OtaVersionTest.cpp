#include <gtest/gtest.h>

#include <fstream>
#include <iterator>
#include <string>

#include "OtaVersion.h"

namespace {

bool isUpdate(const char* latestTag, const char* runningVersion) {
  return OtaVersion::compare(latestTag, runningVersion) > 0;
}

TEST(OtaVersion, OffersTheNextFluidezBuild) {
  EXPECT_TRUE(isUpdate("v1.6-fluidez8", "1.6-fluidez7-x4-pro"));
  EXPECT_TRUE(isUpdate("v1.6-fluidez8", "1.6-fluidez7-sticky"));
  EXPECT_TRUE(isUpdate("v1.6-fluidez8", "1.6-fluidez7-x4-classic"));
  EXPECT_TRUE(isUpdate("v1.6-fluidez8", "1.6-fluidez7"));
}

TEST(OtaVersion, TreatsTheSameFluidezBuildOnAnyDeviceAsCurrent) {
  EXPECT_EQ(OtaVersion::compare("v1.6-fluidez8", "1.6-fluidez8-x4-pro"), 0);
  EXPECT_EQ(OtaVersion::compare("v1.6-fluidez8", "1.6-fluidez8-sticky"), 0);
  EXPECT_EQ(OtaVersion::compare("v1.6-fluidez8", "1.6-fluidez8"), 0);
}

TEST(OtaVersion, RejectsOlderFluidezBuilds) {
  EXPECT_LT(OtaVersion::compare("v1.6-fluidez7", "1.6-fluidez8-x4-pro"), 0);
  EXPECT_FALSE(isUpdate("v1.6-fluidez7", "1.6-fluidez8-x4-pro"));
}

TEST(OtaVersion, ComparesFluidezBuildNumbersNumerically) {
  EXPECT_TRUE(isUpdate("v1.6-fluidez10", "1.6-fluidez9-x4-pro"));
  EXPECT_FALSE(isUpdate("v1.6-fluidez9", "1.6-fluidez10-x4-pro"));
}

TEST(OtaVersion, LetsTheBaseVersionOutrankTheFluidezBuild) {
  EXPECT_TRUE(isUpdate("v1.7-fluidez1", "1.6-fluidez9-x4-pro"));
  EXPECT_FALSE(isUpdate("v1.6-fluidez9", "1.7-fluidez1-x4-pro"));
}

TEST(OtaVersion, PrefersFluidezOverAStockBuildOfTheSameBase) {
  EXPECT_TRUE(isUpdate("v1.6-fluidez8", "1.6-next-x4-pro"));
  EXPECT_FALSE(isUpdate("v1.6.0", "1.6-fluidez8-x4-pro"));
  EXPECT_TRUE(isUpdate("v1.6.1", "1.6-fluidez8-x4-pro"));
}

TEST(OtaVersion, MatchesTheFluidezMarkerWithoutCaseSensitivity) {
  EXPECT_TRUE(isUpdate("V1.6-FluiDez9", "1.6-fluidez8-x4-pro"));
  EXPECT_EQ(OtaVersion::parseFluidezBuild("1.6-FLUIDEZ12"), 12);
}

TEST(OtaVersion, ParsesOnlyPlausibleFluidezBuildNumbers) {
  EXPECT_EQ(OtaVersion::parseFluidezBuild("1.6-fluidez7-x4-pro"), 7);
  EXPECT_EQ(OtaVersion::parseFluidezBuild("1.6-fluidez0"), 0);
  EXPECT_EQ(OtaVersion::parseFluidezBuild("1.6-fluidez"), OtaVersion::NO_FLUIDEZ_BUILD);
  EXPECT_EQ(OtaVersion::parseFluidezBuild("1.6-fluidez1234567"), OtaVersion::NO_FLUIDEZ_BUILD);
  EXPECT_EQ(OtaVersion::parseFluidezBuild("1.6-next-x4-pro"), OtaVersion::NO_FLUIDEZ_BUILD);
  EXPECT_EQ(OtaVersion::parseFluidezBuild(nullptr), OtaVersion::NO_FLUIDEZ_BUILD);
}

TEST(OtaVersion, KeepsUpstreamNumericOrdering) {
  EXPECT_TRUE(isUpdate("v1.6.1", "1.6.0-x4-pro"));
  EXPECT_TRUE(isUpdate("v1.6.0.1", "1.6.0"));
  EXPECT_TRUE(isUpdate("V2.0", "1.9.9.9"));
  EXPECT_FALSE(isUpdate("v1.6.0", "1.6.1"));
  EXPECT_EQ(OtaVersion::compare("v1.6", "1.6.0.0"), 0);
}

TEST(OtaVersion, ReplacesAReleaseCandidateWithItsFinalRelease) {
  EXPECT_TRUE(isUpdate("v1.6-fluidez8", "1.6-fluidez8-rc1-x4-pro"));
  EXPECT_TRUE(isUpdate("v1.6.0", "1.6.0-RC"));
  EXPECT_FALSE(isUpdate("v1.6.0-rc2", "1.6.0"));
}

TEST(OtaVersion, NeverUpdatesFromUnparseableVersions) {
  EXPECT_EQ(OtaVersion::compare("latest", "1.6-fluidez8"), 0);
  EXPECT_EQ(OtaVersion::compare("fluidez9", "1.6-fluidez8"), 0);
  EXPECT_EQ(OtaVersion::compare("v1.6-fluidez9", ""), 0);
  EXPECT_EQ(OtaVersion::compare(nullptr, "1.6-fluidez8"), 0);
  EXPECT_EQ(OtaVersion::compare("v1.6.", "1.5"), 0);
}

TEST(OtaVersion, ChecksFluiDezReaderReleasesForUpdates) {
  std::ifstream source(OTA_UPDATER_SOURCE_PATH);
  const std::string text{std::istreambuf_iterator<char>(source), std::istreambuf_iterator<char>()};
  ASSERT_FALSE(text.empty());

  // Devices only follow the release feed compiled into them, so an upstream sync
  // must not silently point FluiDez firmware back at another repository.
  EXPECT_NE(text.find("\"https://api.github.com/repos/micheljatuba/FluiDez-Reader/releases/latest\""),
            std::string::npos);
  EXPECT_NE(text.find("OtaVersion::compare(latestVersion.c_str(), AppVersion::version())"), std::string::npos);
}

}  // namespace
