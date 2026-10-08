add_executable(PersistableStoreAtomicTest
  PersistableStoreAtomicTest.cpp
  ${REPO_ROOT}/lib/Serialization/PersistableStore.cpp
)

target_include_directories(PersistableStoreAtomicTest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/stubs
  ${REPO_ROOT}/lib/Serialization
)

target_link_libraries(PersistableStoreAtomicTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
  ArduinoJson
)

gtest_discover_tests(PersistableStoreAtomicTest)

add_executable(FluiDezStateTest
  FluiDezStateTest.cpp
  ${REPO_ROOT}/src/FluiDezState.cpp
  ${REPO_ROOT}/lib/Serialization/PersistableStore.cpp
  ${REPO_ROOT}/lib/uzlib/src/uzlib_checksums.c
)
target_include_directories(FluiDezStateTest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/stubs
  ${REPO_ROOT}/src
  ${REPO_ROOT}/lib/Serialization
  ${REPO_ROOT}/lib/uzlib/src
)
target_link_libraries(FluiDezStateTest PRIVATE crosspoint_test_common GTest::gtest_main ArduinoJson)
gtest_discover_tests(FluiDezStateTest)
