include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

get_filename_component(DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(MODULE_NAME ${DIR} NAME)
message("MODULE_NAME: -> ${MODULE_NAME}")



set(UNITY_EXTENSION_FIXTURE ON CACHE INTERNAL "Set UNITY_EXTENSION_FIXTURE CACHE INTERNAL.")
set(UNITY_EXTENSION_MEMORY ON CACHE INTERNAL "Set UNITY_EXTENSION_MEMORY CACHE INTERNAL.")

if(MODULE_NAME STREQUAL "crf")
  set(UNITY_PACKAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/unity)
else()
  set(UNITY_PACKAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../deps/unity)
endif()

if (EXISTS ${UNITY_PACKAGE_PATH})
  set(FETCHCONTENT_FULLY_DISCONNECTED ON)
  message("Unity is already polulated. Skipping download.")
else()
  message("Unity is not yet populated. Downloading...")
endif()

FetchContent_Declare(
  Unity
  GIT_REPOSITORY  https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG         67ca5c57c9c50cc1580518cc6f822558c0009b1e
  SOURCE_DIR      ${UNITY_PACKAGE_PATH}
  FIND_PACKAGE_ARGS
  )

# Fetch the content using previously declared details
FetchContent_MakeAvailable( Unity )


