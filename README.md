Ongoing project to map keyboard keys to a joystick using xdotool and linux joystick api

joystick.h:

read_button_press: reads button press and returns only the integer value of the button

set_key_binding: writes read button presses and binds them to a key

read_map_from_file: read map from file

write_map_to_file: writes map to file

saves map to $HOME/.joystickconfig/map.bin

Makefile:

sudo make : compiles the local library files

sudo make install : installs the library to the system

sudo make clean : deletes local compiled library

sudo make uninstall : uninstalls the library from the system
