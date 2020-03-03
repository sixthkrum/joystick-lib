#include <stdio.h>

#include <iostream>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <unistd.h>

#include <stdlib.h>

#include <string.h>

#include <stdint.h>

#include <fstream>

#include <vector>

#include <array>

#include <iterator>

#include <map>

#include <ncurses.h>

#include <xdo.h>

#define JS_EVENT_BUTTON 0x01

#define JS_EVENT_AXIS 0x02

#define JS_EVENT_INIT 0x80

struct js_event {
  uint32_t time;
  short value;
  uint8_t type;
  uint8_t number;
};

//only returns int for now need to make it change to all kinds of input
//status : n
// use int fd = open ( "/dev/input/js0" , /*for blocking: O_RDONLY , non blocking: O_NONBLOCK*/ O_RDONLY ) to open joystick

std::array < int , 2 > read_button_press( int fd , bool output_enable = 0 , bool report_button_release = 0) {
  if (fd > 0 && output_enable) {
    std::cout << "joystick opened" << '\n';
  }

  else if( fd <= 0 && output_enable) {
    std::cout << "no joystick available" << '\n';
    return { -1 , 0 };
  }

  fd_set rfds;
  timeval tv;
  int retval;

  tv.tv_sec = 0;
  tv.tv_usec = 100;

  FD_ZERO ( &rfds );
  FD_SET ( fd , &rfds );

  retval = select( fd + 1 , &rfds , NULL , NULL , &tv );

  js_event e;

  if ( FD_ISSET ( fd , &rfds) ) {
    read ( fd , &e , sizeof ( e ) );

  std::array < int , 2 > button;

    switch ( e.type ) {
      case JS_EVENT_AXIS: {
        if( output_enable ) {
          std::cout << "AXIS" << '\t';
          std::cout << +e.number << '\t';
          std::cout << +e.value << std::endl;
        }

        if( report_button_release | e.value ) {
          button [0] = int ( e.type );
          button [1] = int ( e.number );
          return button;
        }

        break;
      }

      case JS_EVENT_BUTTON: {
        if( output_enable ) {
          std::cout << "BUTTON" << '\t';
          std::cout << +e.number << '\t';
          std::cout << +e.value << std::endl;
        }

        if( report_button_release | e.value ) {
          button [0] = int ( e.type );
          button [1] = int ( e.number );
          return button;
        }

        break;
      }

      default: {
        break;
      }
    }
  }

  return { -2 , 0 };
}
// need to test this and use xdotool
// need to get what key integer maps to in xdotool from ncurses
int set_key_binding( std::map < std::array < int , 2 > , int >& keybindings) {
  initscr();
  nodelay( stdscr , FALSE );
  curs_set( FALSE );
  keypad( stdscr , TRUE );
  noecho();

  int fd = open ( "/dev/input/js0" , /*for blocking: O_RDONLY , non blocking: O_NONBLOCK*/ O_RDONLY );

  if (fd > 0) {
    printw( "joystick opened" );
    refresh();
  }

  else {
    printw( "no joystick available" );
    refresh();
    endwin();
    return 0;
  }

  //I want to eventualy make a way to have a continuous stream of events available
  //right now i am going to make a basic thing that returns the uint8_t number of the button press
  //
  while ( TRUE ) {
    clear();
    mvprintw( 0 , 0 , "enter key to bind buttons to: " );
    refresh();

    int key;
    key = getch();
    printw( "%d" , key );

    mvprintw( 1 , 0 , "press buttons that will map to the key press ESC to stop: " );
    refresh();

    nodelay( stdscr , TRUE );

    std::array < int , 2 > button;

    while( getch() != 27 ) {
      button = read_button_press( fd );

      switch ( button [0] ) {
        case -2: {
          break;
        }

        case -1: {
          clear();
          mvprintw( 2 , 0 , "the joystick has been disconnected, saving partial map and exiting");
          refresh();
          endwin();
          // need to save map here
          return -1;
        }

        default: {
          printw( "%d\t" , button [1] );
          keybindings.insert( std::pair < std::array < int , 2 > , int > ( button , key ) );
          break;
        }
      }
    }

    clear();
    mvprintw( 3 , 0 , "bind another key? press y to continue");
    refresh();

    nodelay( stdscr , FALSE );

    int choice;

    choice = getch();

    if ( choice == 'y' || choice == 'Y') {
      continue;
    }

    else {
      break;
    }

  }

clear();
endwin();

return 1;
}
