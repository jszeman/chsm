include(FetchContent)

#----------------- PLATFORM ------------------------

FetchContent_Declare(
  Unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        origin/master
  SOURCE_DIR     ${PROJECT_SOURCE_DIR}/modules/unittest/unity
  )

# FetchContent_Declare(
#   chsm
#   GIT_REPOSITORY https://github.com/jszeman/chsm.git
#   GIT_TAG        origin/feature/examples
#   SOURCE_DIR     ${PROJECT_SOURCE_DIR}/chsm
#   )

set(FETCHCONTENT_QUIET OFF)

# FetchContent_MakeAvailable( Unity 
#                   # chsm 
#                   )