add_executable (monitor	main.c
			configuration.c configuration.h
			panic.c panic.h)

target_link_libraries(monitor PkgConfig::BLKID)

install(TARGETS monitor DESTINATION sbin)
