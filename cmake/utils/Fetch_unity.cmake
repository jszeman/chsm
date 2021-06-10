include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
# set(FETCHCONTENT_UPDATE_DISCONNECTED_Unity ON)

get_filename_component(DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(MODULE_NAME ${DIR} NAME)
message("MODULE_NAME: -> ${MODULE_NAME}")

if(MODULE_NAME STREQUAL "crf")
  FetchContent_Declare(
    Unity
    GIT_REPOSITORY  https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG         origin/master
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/unity
    )
else()
  FetchContent_Declare(
    Unity
    GIT_REPOSITORY  https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG         origin/master
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/../../../deps/unity
    )
endif()

set(UNITY_EXTENSION_FIXTURE ON CACHE INTERNAL "Set UNITY_EXTENSION_FIXTURE CACHE INTERNAL.")
set(UNITY_EXTENSION_MEMORY ON CACHE INTERNAL "Set UNITY_EXTENSION_MEMORY CACHE INTERNAL.")
  
FetchContent_MakeAvailable( Unity )
