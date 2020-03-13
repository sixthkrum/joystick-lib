libjoystick.so.1.0.1:	joystick.o
	g++	-shared	-Wl,-soname,libjoystick.so.1	-Wl,--no-undefined	-o	libjoystick.so.1.0.1	joystick.o	-lxdo

joystick.o:	joystick.h
	g++	-fPIC	-g	-c	joystick.cpp

.PHONY: reinstall
reinstall:	uninstall clean install

.PHONY:	install
install: libjoystick.so.1.0.1
	mkdir	~/.joystickconfig
	touch	~/.joystickconfig/map.bin
	chown	-R	user ~/.joystickconfig
	chmod	-R	755 ~/.joystickconfig
	cp	-f	joystick.h	/usr/local/include/joystick.h
	cp	-f	libjoystick.so.1.0.1	/usr/local/lib/libjoystick.so.1.0.1
	ln	-s	/usr/local/lib/libjoystick.so.1.0.1 /usr/local/lib/libjoystick.so
	ldconfig

.PHONY:	uninstall
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
