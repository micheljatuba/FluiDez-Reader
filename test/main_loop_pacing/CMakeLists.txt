set(main_source "${REPO_ROOT}/src/main.cpp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${main_source}")
file(READ "${main_source}" main_contents)
string(FIND "${main_contents}" "void loop() {" loop_start)
if(loop_start EQUAL -1)
  message(FATAL_ERROR "Could not locate the production main loop")
endif()
string(SUBSTRING "${main_contents}" ${loop_start} -1 loop_contents)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/MainLoopUnderTest.inc" "${loop_contents}")

foreach(profile IN ITEMS Device Simulator)
  set(target MainLoopPacing${profile}Test)
  add_executable(${target}
    MainLoopPacingTest.cpp
    ${REPO_ROOT}/src/util/MainLoopPacing.cpp
  )
  target_include_directories(${target} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs
    ${CMAKE_CURRENT_BINARY_DIR}
    ${REPO_ROOT}/src/util
  )
  target_compile_definitions(${target} PRIVATE CROSSINK_APP_CAP_TOUCH=1)
  if(profile STREQUAL "Simulator")
    target_compile_definitions(${target} PRIVATE SIMULATOR)
  endif()
  target_link_libraries(${target} PRIVATE crosspoint_test_common GTest::gtest_main)
  gtest_discover_tests(${target} TEST_PREFIX "${profile}.")
endforeach()
