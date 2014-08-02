add_executable (dmxd dmxd.c misc.c)
find_library (LIBFTDI NAMES ftdi ftdi1)
target_link_libraries (dmxd usb ${LIBFTDI} pthread m dl)
install (TARGETS dmxd DESTINATION bin)

if (${LIBFTDI} MATCHES "ftdi1")
    add_definitions(-DCOMPILED_WITH_FTDI1)
endif ()

