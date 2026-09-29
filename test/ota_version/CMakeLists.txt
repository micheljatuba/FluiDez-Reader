add_executable(OtaVersionTest
  OtaVersionTest.cpp
)

target_include_directories(OtaVersionTest PRIVATE
  ${REPO_ROOT}/src/network
)

target_compile_definitions(OtaVersionTest PRIVATE
  OTA_UPDATER_SOURCE_PATH="${REPO_ROOT}/src/network/OtaUpdater.cpp"
)

target_link_libraries(OtaVersionTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(OtaVersionTest)
