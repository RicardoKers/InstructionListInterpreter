# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SimuladorIL_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SimuladorIL_autogen.dir\\ParseCache.txt"
  "SimuladorIL_autogen"
  )
endif()
