add_custom_command (
	OUTPUT ipc_protocol_generated.h
        COMMAND ${FLATC} -c ${CMAKE_CURRENT_SOURCE_DIR}/../ipc_protocol.fbs
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../ipc_protocol.fbs
	VERBATIM)

set_source_files_properties(ipc_protocol_generated.h PROPERTIES GENERATED TRUE)

add_executable (monitor	main.c
			configuration.c configuration.h
			panic.c panic.h
			queue.c queue.h
			control_loop.cc control_loop.h
			ipc_protocol_generated.h)

target_link_libraries(monitor PkgConfig::BLKID)

target_compile_options(monitor PRIVATE -DVERBOSE=1 -g)

install(TARGETS monitor DESTINATION sbin)
