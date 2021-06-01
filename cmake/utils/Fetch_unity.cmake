include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  Unity
  GIT_REPOSITORY  https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG         origin/master
  SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/unity
  )

set(UNITY_EXTENSION_FIXTURE ON CACHE INTERNAL "Set UNITY_EXTENSION_FIXTURE CACHE INTERNAL.")
set(UNITY_EXTENSION_MEMORY ON CACHE INTERNAL "Set UNITY_EXTENSION_MEMORY CACHE INTERNAL.")
  
FetchContent_MakeAvailable( Unity )
