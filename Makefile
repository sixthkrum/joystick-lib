libxjs.so.1.0.1:	xjs.o
	g++	-shared	-Wl,-soname,libxjs.so.1	-Wl,--no-undefined	-o	libxjs.so.1.0.1	xjs.o	-lxdo

xjs.o:	xjs.h
	g++	-fPIC	-g	-c	xjs.cpp

.PHONY: reinstall
reinstall:	uninstall clean install

.PHONY:	install
install: libxjs.so.1.0.1
	mkdir	~/.xjsconfig
	touch	~/.xjsconfig/map.bin
	chown	-R	user ~/.xjsconfig
	chmod	-R	755 ~/.xjsconfig
	cp	-f	xjs.h	/usr/local/include/xjs.h
	cp	-f	libxjs.so.1.0.1	/usr/local/lib/libxjs.so.1.0.1
	ln	-s	/usr/local/lib/libxjs.so.1.0.1 /usr/local/lib/libxjs.so
	ldconfig

.PHONY:	uninstall
uninstall:
	rm	-rf	~/.xjsconfig
	rm	-f	/usr/local/include/xjs.h
	rm	-f	/usr/local/lib/libxjs.so.1.0.1
	rm	-f	/usr/local/lib/libxjs.so.1
	rm	-f	/usr/local/lib/libxjs.so
	ldconfig

.PHONY:	clean
clean:
	rm	-f	xjs.o
	rm	-f	libxjs.so.1.0.1
