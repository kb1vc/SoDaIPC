# Borrowed from https://wiki.o-ran-sc.org/display/ORAN/Packaging+Libraries+for+Linux+Distributions+with+CMake

IF( EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake" )
  include( InstallRequiredSystemLibraries )

  set( CPACK_SET_DESTDIR "off" )
  set( CPACK_PACKAGING_INSTALL_PREFIX "${install_root}" )
  set(CPACK_GENERATOR "")
  if(BUILD_RPM) 
    list(APPEND CPACK_GENERATOR "RPM" )
  endif()
  if(BUILD_DEB)
    list(APPEND CPACK_GENERATOR "DEB" )    
  endif()
  
  if(PACKAGE_SYSTEM_NAME)
    set(TARGET_SYSNAME "_${PACKAGE_SYSTEM_NAME}")
  endif()

  
  set( CPACK_PACKAGE_DESCRIPTION "SoDa package for string formatting and command-line parsing." )
  set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "SoDa Inter-process (thread) communication stuff." )
  set( CPACK_PACKAGE_VENDOR "kb1vc" )
  set( CPACK_PACKAGE_CONTACT "https://github.com/kb1vc/" )
  set( CPACK_PACKAGE_VERSION_MAJOR "${SoDaIPC_VERSION_MAJOR}")
  set( CPACK_PACKAGE_VERSION_MINOR "${SoDaIPC_VERSION_MINOR}")
  set( CPACK_PACKAGE_VERSION_PATCH "${SoDaIPC_VERSION_PATCH}")
  set( CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${TARGET_SYSNAME}${spoiled_str}" )
  set( CPACK_SOURCE_PACKAGE_FILE_NAME
    "vric${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )

  # omit if not required
  set( CPACK_RPM_PACKAGE_REQUIRES "gcc-g++ >= 5.0.0")
    
  set( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
  set( CPACK_DEBIAN_PACKAGE_SECTION "ric" )
  set( CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} )
  set( CPACK_RPM_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} )
  
  set( MYCMAKE_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake")
  list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION ${MYCMAKE_DIR})

  INCLUDE( CPack )
ENDIF()
