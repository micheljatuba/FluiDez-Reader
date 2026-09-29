#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>

// Pinned-book rules shared by RecentBooksStore and its host tests. Books are
// stored most recent first; pinSequence is 0 when a book is unpinned and grows
// with each pin, so sorting by it keeps pinned books in the order they were pinned.
namespace RecentBooksOrder {

template <typename Book>
size_t pinnedCount(const std::vector<Book>& books) {
  return static_cast<size_t>(
      std::count_if(books.begin(), books.end(), [](const Book& book) { return book.pinSequence != 0; }));
}

template <typename Book>
uint32_t nextPinSequence(const std::vector<Book>& books) {
  uint32_t highest = 0;
  for (const Book& book : books) highest = std::max(highest, book.pinSequence);
  return highest == UINT32_MAX ? highest : highest + 1;
}

// Drops the least recent unpinned books until the list fits.
template <typename Book>
void trimToCapacity(std::vector<Book>& books, const size_t capacity) {
  while (books.size() > capacity) {
    const auto victim =
        std::find_if(books.rbegin(), books.rend(), [](const Book& book) { return book.pinSequence == 0; });
    books.erase(victim == books.rend() ? std::prev(books.end()) : std::next(victim).base());
  }
}

// Fills `order` with indices into `books`: pinned books first in pin order,
// then the rest by recency. Home keeps the most recent book first so Continue
// Reading is unchanged. Returns the number of indices written.
template <size_t Capacity, typename Book>
size_t displayOrder(const std::vector<Book>& books, const bool keepMostRecentFirst,
                    std::array<uint8_t, Capacity>& order) {
  static_assert(Capacity <= 256, "indices are stored as uint8_t");
  size_t count = 0;
  const auto append = [&](const size_t index) {
    if (count < Capacity && index <= UINT8_MAX) order[count++] = static_cast<uint8_t>(index);
  };

  const size_t firstCandidate = keepMostRecentFirst && !books.empty() ? 1 : 0;
  if (firstCandidate == 1) append(0);

  // Select pinned books by (sequence, index) so a short order still keeps the earliest pins.
  bool havePrevious = false;
  uint32_t previousSequence = 0;
  size_t previousIndex = 0;
  while (count < Capacity) {
    size_t next = books.size();
    for (size_t i = firstCandidate; i < books.size(); ++i) {
      const uint32_t sequence = books[i].pinSequence;
      if (sequence == 0) continue;
      if (havePrevious && (sequence < previousSequence || (sequence == previousSequence && i <= previousIndex))) {
        continue;
      }
      if (next == books.size() || sequence < books[next].pinSequence) next = i;
    }
    if (next == books.size()) break;
    append(next);
    havePrevious = true;
    previousSequence = books[next].pinSequence;
    previousIndex = next;
  }

  for (size_t i = firstCandidate; i < books.size(); ++i) {
    if (books[i].pinSequence == 0) append(i);
  }
  return count;
}

}  // namespace RecentBooksOrder
