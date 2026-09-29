#include <HalStorage.h>
#include <gtest/gtest.h>

#include <string>

#include "BookReadingStats.h"

HalStorage Storage;

// Date bucketing lives in ReadingStatsUtils.cpp, which needs the RTC; persistence does not.
bool ReadingStatsDate::isValid() const { return year != 0 && month != 0 && day != 0; }
void ReadingStatsDate::clear() { *this = ReadingStatsDate{}; }
void recordReadingSpanIntoBuckets(std::array<uint32_t, READING_TIME_BUCKET_COUNT>&,
                                  std::array<uint32_t, READING_DAY_OF_WEEK_COUNT>&, const ReadingStatsDateTime&,
                                  uint32_t) {}

namespace {

const std::string kCachePath = "/.crosspoint/epub_1";
const std::string kStatsPath = kCachePath + "/stats_v5.bin";
const std::string kTempPath = kStatsPath + ".tmp";

BookReadingStats withSessions(const uint16_t sessions) {
  BookReadingStats stats;
  stats.sessionCount = sessions;
  stats.totalReadingSeconds = sessions * 60U;
  return stats;
}

class BookReadingStatsSaveTest : public testing::Test {
 protected:
  void SetUp() override { Storage = HalStorage{}; }

  static void saveWithFailedPublish(const uint16_t sessions) {
    Storage.fs.failRename = true;
    withSessions(sessions).save(kCachePath);
    Storage.fs.failRename = false;
  }
};

TEST_F(BookReadingStatsSaveTest, SaveAndLoadRoundTrip) {
  withSessions(3).save(kCachePath);

  const BookReadingStats loaded = BookReadingStats::load(kCachePath);
  EXPECT_EQ(loaded.sessionCount, 3);
  EXPECT_EQ(loaded.totalReadingSeconds, 180U);
  EXPECT_TRUE(Storage.exists(kStatsPath.c_str()));
  EXPECT_FALSE(Storage.exists(kTempPath.c_str()));
}

TEST_F(BookReadingStatsSaveTest, FailedPublishKeepsTheNewCompleteCopy) {
  withSessions(1).save(kCachePath);
  saveWithFailedPublish(2);

  EXPECT_EQ(BookReadingStats::load(kCachePath).sessionCount, 2);
}

TEST_F(BookReadingStatsSaveTest, NextSaveRepublishesAfterAFailedPublish) {
  withSessions(1).save(kCachePath);
  saveWithFailedPublish(2);
  withSessions(3).save(kCachePath);

  EXPECT_TRUE(Storage.exists(kStatsPath.c_str()));
  EXPECT_FALSE(Storage.exists(kTempPath.c_str()));
  EXPECT_EQ(BookReadingStats::load(kCachePath).sessionCount, 3);
}

TEST_F(BookReadingStatsSaveTest, RemoveAlsoDeletesAnUnpublishedCopy) {
  withSessions(1).save(kCachePath);
  saveWithFailedPublish(2);

  EXPECT_TRUE(BookReadingStats::remove(kCachePath));
  EXPECT_FALSE(Storage.exists(kTempPath.c_str()));
  EXPECT_EQ(BookReadingStats::load(kCachePath).sessionCount, 0);
}

TEST_F(BookReadingStatsSaveTest, FailedReplaceKeepsThePreviousCopy) {
  withSessions(1).save(kCachePath);
  Storage.fs.failRemovePath = kStatsPath;
  withSessions(2).save(kCachePath);
  Storage.fs.failRemovePath.clear();

  EXPECT_EQ(BookReadingStats::load(kCachePath).sessionCount, 1);
  EXPECT_FALSE(Storage.exists(kTempPath.c_str()));
}

}  // namespace
