ldconfig: libjoystick.so.1.0.1	libjoystick.so
	ldconfig

libjoystick.so:	libjoystick.so.1.0.1
	ln	-s	/usr/local/lib/libjoystick.so.1.0.1 /usr/local/lib/libjoystick.so

libjoystick.so.1.0.1:	joystick.o
	g++	-shared	-Wl,-soname,libjoystick.so.1	-o	libjoystick.so.1.0.1	joystick.o	-lncurses
	cp	-f	libjoystick.so.1.0.1	/usr/local/lib/libjoystick.so.1.0.1

joystick.o:	joystick.h
	g++	-fPIC	-g	-c	joystick.cpp

joystick.h:
	cp	-f	joystick.h	/usr/local/include/joystick.h

.PHONY:	clean
clean:
	rm	-f	/usr/local/include/joystick.h
	rm	-f	joystick.o
	rm	-f	libjoystick.so.1.0.1
	rm	-f	libjoystick.so
	rm	-f	/usr/local/lib/libjoystick.so.1.0.1
	rm	-f	/usr/local/lib/libjoystick.so.1
	rm	-f	/usr/local/lib/libjoystick.so
	ldconfig
