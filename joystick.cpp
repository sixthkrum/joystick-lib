#include "joystick.h"

std::array < int , 3 > read_button_press( int fd , bool output_enable , bool report_button_release) {
  if (fd > 0 && output_enable) {
    std::cout << "joystick opened" << '\n';
  }

  else if( fd <= 0 && output_enable) {
    std::cout << "no joystick available" << '\n';
    return { -1 , 0 , 0 };
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

  std::array < int , 3 > button;

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
          button [2] = int ( e.value );
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
          button [2] = int ( e.value );
          return button;
        }

        break;
      }

      default: {
        break;
      }
    }
  }

  return { -2 , 0 , 0 };
}

int set_key_binding( std::map < std::array < int , 3 > , int >& keybindings) {
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

    std::array < int , 3 > button;

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
          printw( "\n%d\t%d\t%d\t" , button [0] , button [1] , button [2] );
          keybindings.insert( std::pair < std::array < int , 3 > , int > ( button , key ) );
          break;
        }
      }
    }

    clear();
    mvprintw( 0 , 0 , "bind another key? press y to continue");
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
