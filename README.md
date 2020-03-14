Ongoing project to map keyboard keys to a joystick using xdotool and linux joystick api

to compile using this link both libxdo and libxjs

dependencies:

LIBXDO: https://github.com/jordansissel/xdotool follow installation instructions there

xjs.h:

(look at file for more information)

read_button_press: reads button press and returns value type and release or pressed state

set_key_binding: writes read button presses or releases and binds them to a key

read_map_from_file: reads map from file

write_map_to_file: writes map to file

simulate_mapped_key: sends bound key ( or sequence ) to specified xdo instance and window

saves map to $HOME/.xjsconfig/map.bin

Makefile:

make : compiles the local library files

sudo make install : installs the library to the system

make clean : deletes local compiled library

sudo make uninstall : uninstalls the library from the system

sudo make reinstall : uninstalls, cleans then installs the library
