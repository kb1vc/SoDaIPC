
########### next target ###############

set(IPC_SRCS
	MailBox.cxx
	)


add_library(sodaIPC STATIC ${IPC_SRCS})
set_target_properties(sodaIPC PROPERTIES VERSION ${SoDaIPC_VERSION})

install(TARGETS sodaIPC DESTINATION ${LIBDIR})

# Export header files that are required to build loadable modules

install(DIRECTORY ../include/
		  DESTINATION "include/SoDa"
		  FILES_MATCHING PATTERN "*.hxx")


