# Find the SoDaIPC library
# This defines:

#  SoDaIPC_INCLUDE_DIR contains SoDa/*.h
#  SoDaIPC_LIBRARIES points to libsodaipc...
#  SoDaIPC_FOUND true if we found it. 

message("In FindSoDaIPC.cmake")

find_path(SoDaIPC_INCLUDE_DIR
  NAMES SoDa/Format.hxx
  )

find_library(SoDaIPC_LIBRARY sodaipc)


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SoDaIPC
  REQUIRED_VARS SoDaIPC_LIBRARY SoDaIPC_INCLUDE_DIR)

if(SoDaIPC_FOUND)
  set( SoDaIPC_LIBRARIES ${SoDaIPC_LIBRARY} )
endif()

mark_as_advanced(SoDaIPC_INCLUDE_DIR SoDaIPC_LIBRARY)
