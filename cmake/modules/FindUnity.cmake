include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

### Library options ############################################################################

set(UNITY_EXTENSION_FIXTURE ON CACHE INTERNAL "Set UNITY_EXTENSION_FIXTURE CACHE INTERNAL.")
set(UNITY_EXTENSION_MEMORY ON CACHE INTERNAL "Set UNITY_EXTENSION_MEMORY CACHE INTERNAL.")
    
###  

FetchContent_Declare(
  Unity
  GIT_REPOSITORY  https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG         origin/master
  SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/unity
  )
    
FetchContent_GetProperties( Unity )     
FetchContent_MakeAvailable( Unity )

if(NOT Unity_POPULATED)
  FetchContent_Populate(Unity)
  add_subdirectory(${Unity_SOURCE_DIR} ${Unity_BINARY_DIR} EXCLUDE_FROM_ALL)
endif() 
