add_executable (dmxd dmxd.c misc.c)
target_link_libraries (dmxd usb ftdi1 pthread m dl)
install (TARGETS dmxd DESTINATION bin)
