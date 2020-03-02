#include <stdio.h>

#include <iostream>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <unistd.h>

#include <stdlib.h>

#include <string.h>

#include <stdint.h>

#include <iterator>

#include <map>

#define JS_EVENT_BUTTON 0x01

#define JS_EVENT_AXIS 0x02

#define JS_EVENT_INIT 0x80

struct js_event {
  uint32_t time;
  short value;
  uint8_t type;
  uint8_t number;
};

int main()
{
  int fd = open ( "/dev/input/js0" , /*for blocking: O_RDONLY , non blocking: O_NONBLOCK*/ O_RDONLY );

  if (fd > 0) {
    std::cout << "joystick opened" << '\n';
  }

  else {
    std::cout << "error" << '\n';
    return 0;
  }

  fd_set rfds;
  timeval tv;
  int retval;

  tv.tv_sec = 0;
  tv.tv_usec = 100;

  js_event e;

  while (1) {

    FD_ZERO ( &rfds );
    FD_SET ( fd , &rfds );

    retval = select( fd + 1 , &rfds , NULL , NULL , &tv );

    if ( FD_ISSET ( fd , &rfds) ) {
      read ( fd , &e , sizeof ( e ) );

      switch ( e.type ) {
        case JS_EVENT_AXIS: {
          std::cout << "AXIS" << '\t';
          std::cout << +e.number << '\t';
          std::cout << +e.value << std::endl;
          break;
        }

        case JS_EVENT_BUTTON: {
          std::cout << "BUTTON" << '\t';
          std::cout << +e.number << '\t';
          std::cout << +e.value << std::endl;
          break;
        }

        default: {
          break;
        }

      }

    }

  }

  return 1;
}
