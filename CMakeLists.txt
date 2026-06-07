cmake_minimum_required(VERSION 3.20)

cmake_policy(SET CMP0077 NEW)

project(QuickBroadcastor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(FLTK_BUILD_TEST OFF)
set(FLTK_BUILD_EXAMPLES OFF)
set(FLTK_BUILD_SHARED_LIBS OFF)
set(FLTK_BUILD_FLUID OFF)
set(FLTK_BUILD_FLTK_OPTIONS OFF)
set(FLTK_BUILD_GL OFF)
set(FLTK_OPTION_STD ON)
FetchContent_Declare(
    fltk
    GIT_REPOSITORY https://github.com/fltk/fltk.git
    GIT_TAG release-1.4.0
)
FetchContent_MakeAvailable(fltk)

FetchContent_Declare(
    zserge_tray
    GIT_REPOSITORY https://github.com/zserge/tray.git
    GIT_TAG master
)
FetchContent_MakeAvailable(zserge_tray)

set(EESS_ENABLE_CODECVT ON)
add_subdirectory(vendor/Eati/Essentials)

find_package(asio CONFIG REQUIRED)
find_package(reflectcpp CONFIG REQUIRED)

add_executable(QuickBroadcastor
    src/os_main.cxx
    src/main.cxx
    src/msg.hxx
    src/msg.cxx
    src/networkFunctionality.hxx
    src/networkFunctionality.cxx
    src/notifyWin.hxx
    src/notifyWin.cxx
    src/sendWin.hxx
    src/sendWin.cxx
    src/stdafx.cpp
    src/stdafx.h
    src/trayFunctionality.hxx
    src/trayFunctionality.cxx
)

target_precompile_headers(QuickBroadcastor PRIVATE src/stdafx.h)

target_compile_options(QuickBroadcastor PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<C_COMPILER_ID:MSVC>:/utf-8>
)

target_include_directories(QuickBroadcastor PRIVATE ${zserge_tray_SOURCE_DIR})

target_compile_definitions(QuickBroadcastor PRIVATE
    $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0601>
    $<$<PLATFORM_ID:Windows>:TRAY_WINAPI=1>
    $<$<PLATFORM_ID:Darwin>:TRAY_APPKIT=1>
    $<$<PLATFORM_ID:Linux>:TRAY_APPINDICATOR=1>
)

set_target_properties(QuickBroadcastor PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(QuickBroadcastor PRIVATE
    EatiEssentials
    EatiEssentials::CodeCvt
    asio::asio
    fltk::fltk
    reflectcpp::reflectcpp
)
