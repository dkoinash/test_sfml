################################################################################
# Compilation flags
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # Debug
else()
  # Release
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") # using Visual Studio C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ob2 /Oi /Ot")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # using GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") # using Clang
    if (CMAKE_CXX_COMPILER MATCHES ".+clang-cl.+") # using Clang-CL
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ob2 /Oi /Ot")
    else() # using Clang GNU
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    endif()
  endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") # using Visual Studio C++
  # Включает макрос __cplusplus, чтобы сообщить о поддерживаемом стандарте 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") # using Clang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undefined-var-template -Wno-switch")

  if (CMAKE_CXX_COMPILER MATCHES ".+clang-cl.+") # using Clang-CL
    target_link_libraries(${PROJECT_NAME} libomp.lib)
  endif()
endif()