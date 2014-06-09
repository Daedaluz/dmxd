add_executable (dmxd dmxd.c misc.c)
target_link_libraries (dmxd usb ftdi pthread m dl)
install (TARGETS dmxd DESTINATION bin)
