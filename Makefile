libjoystick.so.1.0.1:	joystick.o
	g++	-shared	-Wl,-soname,libjoystick.so.1	-o	libjoystick.so.1.0.1	joystick.o	-lncurses

joystick.o:	joystick.h
	g++	-fPIC	-g	-c	joystick.cpp

install: libjoystick.so.1.0.1
	mkdir	~/.joystickconfig
	touch	~/.joystickconfig/map.bin
	cp	-f	joystick.h	/usr/local/include/joystick.h
	cp	-f	libjoystick.so.1.0.1	/usr/local/lib/libjoystick.so.1.0.1
	ln	-s	/usr/local/lib/libjoystick.so.1.0.1 /usr/local/lib/libjoystick.so
	ldconfig

uninstall:
	rm	-rf	~/.joystickconfig
	rm	-f	/usr/local/include/joystick.h
	rm	-f	/usr/local/lib/libjoystick.so.1.0.1
	rm	-f	/usr/local/lib/libjoystick.so.1
	rm	-f	/usr/local/lib/libjoystick.so
	ldconfig

.PHONY:	clean
clean:
	rm	-f	joystick.o
	rm	-f	libjoystick.so.1.0.1
	rm	-f	libjoystick.so
