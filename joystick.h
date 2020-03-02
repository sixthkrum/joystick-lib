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

#include <iterator>

#include <map>

#include <ncurses.h>


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

int read_button_press( int fd , bool output_enable = 1) {
  if (fd > 0 && output_enable) {
    std::cout << "joystick opened" << '\n';
  }

  else if( fd <= 0 && output_enable) {
    std::cout << "no joystick available" << '\n';
    return -1;
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

    switch ( e.type ) {
      case JS_EVENT_AXIS: {
        if( output_enable ) {
          std::cout << "AXIS" << '\t';
          std::cout << +e.number << '\t';
          std::cout << +e.value << std::endl;
        }
        return int( e.number );
        break;
      }

      case JS_EVENT_BUTTON: {
        if( output_enable ) {
          std::cout << "BUTTON" << '\t';
          std::cout << +e.number << '\t';
          std::cout << +e.value << std::endl;
        }
        return int( e.number );
        break;
      }

      default: {
        break;
      }
    }
  }

  return -2;
}
// need to test this and use xdotool
// need to get what key integer maps to in xdotool from ncurses
int set_key_binding( std::map < int , int >& keybindings) {
  int fd = open ( "/dev/input/js0" , /*for blocking: O_RDONLY , non blocking: O_NONBLOCK*/ O_RDONLY );

  if (fd > 0) {
    std::cout << "joystick opened" << '\n';
  }

  else {
    std::cout << "no joystick available" << '\n';
    return 0;
  }

  initscr();
  clear();
  nodelay( stdscr , FALSE );
  curs_set( FALSE );
  keypad( stdscr , TRUE );
  noecho();
  //I want to eventualy make a way to have a continuous stream of events available
  //right now i am going to make a basic thing that returns the uint8_t number of the button press
  //
  while ( TRUE ) {
    mvprintw( 0 , 0 , "enter key to bind buttons to: " );
    refresh();

    int key;
    key = getch();
    printw( "%d" , key );

    mvprintw( 1 , 0 , "press buttons that will map to the key press ESC to stop: " );
    refresh();

    std::vector < int > buttons_pressed;

    nodelay( stdscr , TRUE );

    int button;

    while( getch() != 27 ) {
      button = read_button_press( fd , 1 );

      switch ( button ) {
        case -2: {
          break;
        }

        case -1: {
          mvprintw( 2 , 0 , "the joystick has been disconnected, saving partial map and exiting");
          refresh();
          endwin();
          // need to save map here
          return -1;
        }

        default: {
          printw( "\t%d" , button);
          buttons_pressed.push_back ( button );
          break;
        }
      }
    }

    while ( !buttons_pressed.empty() ) {

      keybindings.insert( std::pair < int , int > ( buttons_pressed.back() , key));
      buttons_pressed.pop_back();
      }

    std::vector < int > ().swap( buttons_pressed );

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
