add_custom_command (
	OUTPUT ipc_protocol_generated.h
	COMMAND ${FLATC} -c ../ipc_protocol.fbs
	VERBATIM)

add_executable (monitor	main.c
			configuration.c configuration.h
			panic.c panic.h
			fifo.c fifo.h
			control_loop.cc control_loop.h
			ipc_protocol_generated.h)

target_link_libraries(monitor PkgConfig::BLKID)

install(TARGETS monitor DESTINATION sbin)
