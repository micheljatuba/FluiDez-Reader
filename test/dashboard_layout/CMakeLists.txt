add_executable(DashboardLayoutTest DashboardLayoutTest.cpp
  ${REPO_ROOT}/src/components/FluiDezBrand.cpp
  ${REPO_ROOT}/src/components/themes/dashboard/DashboardLayout.cpp
  ${REPO_ROOT}/src/components/themes/lyra/LyraGridLayout.cpp
  ${REPO_ROOT}/lib/GfxRenderer/GfxRenderer.cpp
  ${REPO_ROOT}/lib/GfxRenderer/Bitmap.cpp
  ${REPO_ROOT}/lib/GfxRenderer/BitmapHelpers.cpp
  ${REPO_ROOT}/lib/GfxRenderer/FontCacheManager.cpp
  ${REPO_ROOT}/lib/EpdFont/EpdFont.cpp
  ${REPO_ROOT}/lib/EpdFont/EpdFontFamily.cpp
  ${REPO_ROOT}/lib/InflateReader/InflateReader.cpp
  ${REPO_ROOT}/lib/EpdFont/FontDecompressor.cpp
  ${REPO_ROOT}/lib/EpdFont/SdCardFont.cpp
  ${REPO_ROOT}/lib/Memory/BuildScratch.cpp
  ${REPO_ROOT}/lib/Utf8/Utf8.cpp
  ${REPO_ROOT}/lib/MiniBidi/BidiUtils.cpp
  ${REPO_ROOT}/lib/MiniBidi/minibidi.c
  ${REPO_ROOT}/lib/uzlib/src/tinflate.c
  ${REPO_ROOT}/lib/uzlib/src/uzlib_checksums.c)
target_include_directories(DashboardLayoutTest PRIVATE
  ${REPO_ROOT}/src ${REPO_ROOT}/lib/GfxRenderer
  ../epub_grayscale/raster_stubs ../memory_policy/stubs
  ${REPO_ROOT}/freeink-sdk/libs/display/FreeInkDisplay/include
  ${REPO_ROOT}/lib/EpdFont ${REPO_ROOT}/lib/Memory ${REPO_ROOT}/lib/Utf8
  ${REPO_ROOT}/lib/MiniBidi ${REPO_ROOT}/lib/InflateReader ${REPO_ROOT}/lib/uzlib/src)
target_compile_definitions(DashboardLayoutTest PRIVATE ARDUINO_ARCH_ESP32=1 FLUIDEZ_MEMORY_TEST=1)
target_link_libraries(DashboardLayoutTest PRIVATE crosspoint_test_common GTest::gtest_main)
gtest_discover_tests(DashboardLayoutTest PROPERTIES ENVIRONMENT "UBSAN_OPTIONS=halt_on_error=1")
