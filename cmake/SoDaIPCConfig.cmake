#
#  Copyright (c) 2019 Matthew H. Reilly (kb1vc)
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#
#  Redistributions of source code must retain the above copyright
#  notice, this list of conditions and the following disclaimer.
#  Redistributions in binary form must reproduce the above copyright
#  notice, this list of conditions and the following disclaimer in
#  the documentation and/or other materials provided with the
#  distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

set(SoDaIPC_FOUND TRUE)

find_package(PkgConfig QUIET)

set(SoDaIPC_INCLUDE_HINTS)
set(SoDaIPC_LIB_HINTS)



find_path(SoDaIPC_INCLUDE_DIRS
  NAMES SoDa/IPC.hxx
  HINTS ${SoDaIPC_INCLUDE_HINTS}
  PATHS /usr/local/include
        /usr/include
	/opt/local/include
)

find_library(SoDaIPC_LIBRARIES
  NAMES sodaipc
  HINTS ${SoDaIPC_LIB_HINTS}
  PATHS /usr/local/lib
       /usr/lib
       /usr/local/lib64
       /usr/lib64
       /opt/local/lib
)

if(SoDaIPC_INCLUDE_DIRS AND SoDaIPC_LIBRARIES)
  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(SoDaIPC DEFAULT_MSG SoDaIPC_LIBRARIES SoDaIPC_INCLUDE_DIRS)
  mark_as_advanced(SoDaIPC_LIBRARIES SoDaIPC_INCLUDE_DIRS)
elseif(SoDaIPC_FIND_REQUIRED)
  message(FATAL_ERROR "SoDaIPC lib is required, but not found.")
endif()

