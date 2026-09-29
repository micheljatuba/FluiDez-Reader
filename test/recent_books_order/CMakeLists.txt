add_executable(RecentBooksOrderTest
  RecentBooksOrderTest.cpp
)

target_include_directories(RecentBooksOrderTest PRIVATE
  ${REPO_ROOT}/src
)

target_link_libraries(RecentBooksOrderTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(RecentBooksOrderTest)
