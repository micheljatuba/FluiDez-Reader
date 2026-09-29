#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "RecentBooksOrder.h"

namespace {

struct Book {
  std::string path;
  uint32_t pinSequence = 0;
};

// Most recent first, as RecentBooksStore persists them.
std::vector<Book> library() { return {{"current"}, {"study", 2}, {"previous"}, {"reference", 1}, {"older"}}; }

std::vector<std::string> ordered(const std::vector<Book>& books, const bool keepMostRecentFirst) {
  std::array<uint8_t, 18> order{};
  const size_t count = RecentBooksOrder::displayOrder(books, keepMostRecentFirst, order);
  std::vector<std::string> paths;
  for (size_t i = 0; i < count; ++i) paths.push_back(books[order[i]].path);
  return paths;
}

std::vector<std::string> paths(const std::vector<Book>& books) {
  std::vector<std::string> result;
  for (const auto& book : books) result.push_back(book.path);
  return result;
}

TEST(RecentBooksOrder, WithoutPinsBothViewsKeepRecency) {
  const std::vector<Book> books = {{"a"}, {"b"}, {"c"}};
  EXPECT_EQ(ordered(books, false), (std::vector<std::string>{"a", "b", "c"}));
  EXPECT_EQ(ordered(books, true), (std::vector<std::string>{"a", "b", "c"}));
}

TEST(RecentBooksOrder, RecentBooksListsPinnedBooksFirstInPinOrder) {
  EXPECT_EQ(ordered(library(), false),
            (std::vector<std::string>{"reference", "study", "current", "previous", "older"}));
}

TEST(RecentBooksOrder, HomeKeepsTheCurrentBookBeforePinnedBooks) {
  EXPECT_EQ(ordered(library(), true), (std::vector<std::string>{"current", "reference", "study", "previous", "older"}));
}

TEST(RecentBooksOrder, ReadingAPinnedBookDoesNotDuplicateIt) {
  const std::vector<Book> books = {{"study", 2}, {"previous"}, {"reference", 1}};
  EXPECT_EQ(ordered(books, true), (std::vector<std::string>{"study", "reference", "previous"}));
  EXPECT_EQ(ordered(books, false), (std::vector<std::string>{"reference", "study", "previous"}));
}

TEST(RecentBooksOrder, SmallCapacityKeepsPriorityOrder) {
  std::array<uint8_t, 2> order{};
  const auto books = library();
  ASSERT_EQ(RecentBooksOrder::displayOrder(books, true, order), 2U);
  EXPECT_EQ(books[order[0]].path, "current");
  EXPECT_EQ(books[order[1]].path, "reference");
}

TEST(RecentBooksOrder, EmptyListProducesNoEntries) {
  std::array<uint8_t, 18> order{};
  EXPECT_EQ(RecentBooksOrder::displayOrder(std::vector<Book>{}, true, order), 0U);
  EXPECT_EQ(RecentBooksOrder::displayOrder(std::vector<Book>{}, false, order), 0U);
}

TEST(RecentBooksOrder, DuplicatePinSequencesKeepEveryBookInRecencyOrder) {
  const std::vector<Book> books = {{"a"}, {"b", 7}, {"c", 7}, {"d", 3}};
  EXPECT_EQ(ordered(books, false), (std::vector<std::string>{"d", "b", "c", "a"}));
}

TEST(RecentBooksOrder, TrimEvictsTheOldestUnpinnedBook) {
  std::vector<Book> books = {{"new"}, {"a"}, {"pinned", 1}, {"b"}};
  RecentBooksOrder::trimToCapacity(books, 3);
  EXPECT_EQ(paths(books), (std::vector<std::string>{"new", "a", "pinned"}));

  books.insert(books.begin(), Book{"newer"});
  RecentBooksOrder::trimToCapacity(books, 3);
  EXPECT_EQ(paths(books), (std::vector<std::string>{"newer", "new", "pinned"}));
}

TEST(RecentBooksOrder, TrimFallsBackToTheOldestEntryWhenEverythingIsPinned) {
  std::vector<Book> books = {{"a", 3}, {"b", 1}, {"c", 2}};
  RecentBooksOrder::trimToCapacity(books, 2);
  EXPECT_EQ(paths(books), (std::vector<std::string>{"a", "b"}));
}

TEST(RecentBooksOrder, NewPinsFollowTheHighestSequenceAfterUnpinning) {
  std::vector<Book> books = {{"a", 4}, {"b"}, {"c", 2}};
  EXPECT_EQ(RecentBooksOrder::pinnedCount(books), 2U);
  EXPECT_EQ(RecentBooksOrder::nextPinSequence(books), 5U);
  books[0].pinSequence = 0;
  EXPECT_EQ(RecentBooksOrder::pinnedCount(books), 1U);
  EXPECT_EQ(RecentBooksOrder::nextPinSequence(books), 3U);
  EXPECT_EQ(RecentBooksOrder::nextPinSequence(std::vector<Book>{{"max", UINT32_MAX}}), UINT32_MAX);
}

}  // namespace
