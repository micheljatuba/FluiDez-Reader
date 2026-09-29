add_executable(ActivityPowerPolicyGuardTest
  ActivityPowerPolicyGuardTest.cpp
)

target_compile_definitions(ActivityPowerPolicyGuardTest PRIVATE
  ACTIVITIES_SOURCE_DIR="${REPO_ROOT}/src/activities"
)

target_link_libraries(ActivityPowerPolicyGuardTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(ActivityPowerPolicyGuardTest)
