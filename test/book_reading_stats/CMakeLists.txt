add_executable(BookReadingStatsSaveTest
  BookReadingStatsSaveTest.cpp
  ${REPO_ROOT}/src/activities/reader/BookReadingStats.cpp
)

target_include_directories(BookReadingStatsSaveTest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/stubs
  ${REPO_ROOT}/src/activities/reader
)

target_link_libraries(BookReadingStatsSaveTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(BookReadingStatsSaveTest)
