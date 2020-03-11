#include "joystick.h"

bool write_map_to_file ( std::map < std::array < int , 3 > , int > keybindings , bool append_flag )
{
  std::fstream map_file;

  if ( append_flag )
  {
    map_file.open ( MAP_PATH , std::ios::binary | std::ios::out | std::ios::app );
  }

  else
  {
    remove ( MAP_PATH );
    map_file.open ( MAP_PATH , std::ios::binary | std::ios::out );
  }
//replace map path with some macro to find path of directory and then append path of joystcick
  std::map < std::array < int , 3 > , int > :: iterator i;

  for ( i = keybindings.begin() ; i != keybindings.end() ; i ++ )
  {
    map_file.write ( ( char* ) & i -> first [0] , sizeof ( int ) );
    map_file.write ( ( char* ) & i -> first [1] , sizeof ( int ) );
    map_file.write ( ( char* ) & i -> first [2] , sizeof ( int ) );
    map_file.write ( ( char* ) & i -> second , sizeof ( int ) );
  }

  map_file.close();

  return 1; //look into serialization here
}

bool read_map_from_file ( std::map < std::array < int , 3 > , int >& keybindings )
{
  std::fstream map_file;
  map_file.open ( MAP_PATH , std::ios::binary | std::ios::in );

  std::array < int , 3 > button;
  int key;

  while ( map_file.peek() != EOF )
  {
    map_file.read ( ( char* ) & button [0] , sizeof ( int ) );
    map_file.read ( ( char* ) & button [1] , sizeof ( int ) );
    map_file.read ( ( char* ) & button [2] , sizeof ( int ) );
    map_file.read ( ( char* ) & key , sizeof ( int ) );
    keybindings.insert ( std::pair < std::array < int , 3 > , int > ( button , key ) );
  }

  return 1;//need to do the same path stuff here
}

std::array < int , 3 > read_button_press ( int fd , bool output_enable , bool report_button_release )
{
  if ( fd > 0 && output_enable )
  {
    std::cout << "joystick opened" << '\n';
  }

  else if ( fd <= 0 && output_enable )
  {
    std::cout << "no joystick available" << '\n';
    usleep ( 500000 ); //0.5 second sleep
    return { -1 , 0 , 0 };
  }

  fd_set rfds;
  timeval tv;
  int retval;

  tv.tv_sec = 0;
  tv.tv_usec = 100;

  FD_ZERO ( &rfds );
  FD_SET ( fd , &rfds );

  retval = select ( fd + 1 , &rfds , NULL , NULL , &tv );

  js_event e;

  if ( FD_ISSET ( fd , &rfds ) )
  {
    read ( fd , &e , sizeof ( e ) );

    std::array < int , 3 > button;

    switch ( e.type )
    {
      case JS_EVENT_AXIS:
      {
        if ( output_enable )
        {
          std::cout << "AXIS" << '\t';
          std::cout << +e.number << '\t';
          std::cout << +e.value << std::endl;
        }

        if ( report_button_release | e.value )
        {
          button [0] = int ( e.type );
          button [1] = int ( e.number );
          button [2] = int ( e.value );
          return button;
        }
      } break;

      case JS_EVENT_BUTTON:
      {
        if ( output_enable )
        {
          std::cout << "BUTTON" << '\t';
          std::cout << +e.number << '\t';
          std::cout << +e.value << std::endl;
        }

        if ( report_button_release | e.value )
        {
          button [0] = int ( e.type );
          button [1] = int ( e.number );
          button [2] = int ( e.value );
          return button;
        }
      } break;

      default:
      {
      } break;

    }
  }

  return { -2 , 0 , 0 };
}

bool set_key_binding ( std::map < std::array < int , 3 > , int >& keybindings , int fd )
{
  initscr();
  nodelay ( stdscr , FALSE );
  curs_set ( FALSE );
  keypad ( stdscr , TRUE );
  noecho();

  if (fd > 0)
  {
    printw ( "joystick opened" );
    refresh();
    usleep ( 500000 ); //0.5 second sleep
  }

  else
  {
    printw ( "no joystick available" );
    refresh();
    usleep ( 500000 ); //0.5 second sleep
    endwin();
    return 0;
  }

  while ( TRUE )
  {
    clear();
    mvprintw ( 0 , 0 , "enter key to bind buttons to: " );
    refresh();

    int key;
    key = getch();
    printw ( "%d" , key );

    mvprintw ( 1 , 0 , "press buttons that will map to the key press ESC to stop: " );
    refresh();

    nodelay ( stdscr , TRUE );

    std::array < int , 3 > button;

    while ( getch() != 27 )
    {
      button = read_button_press ( fd );

      switch ( button [0] )
      {
        case -2:
        {//handle error here
        } break;

        case -1:
        {
          clear();
          mvprintw ( 2 , 0 , "the joystick has been disconnected, exiting" );
          refresh();
          usleep ( 500000 ); //0.5 second sleep
          endwin();

          return -1;
        } break;

        default:
        {
          printw ( "\n%d\t%d\t%d\t" , button [0] , button [1] , button [2] );
          keybindings.insert ( std::pair < std::array < int , 3 > , int > ( button , key ) );
        } break;

      }
    }

    clear();
    mvprintw ( 0 , 0 , "bind another key? press y to continue" );
    refresh();

    nodelay ( stdscr , FALSE );

    int choice;

    choice = getch();

    if ( choice == 'y' || choice == 'Y' )
    {
      continue;
    }

    else
    {
      break;
    }

  }

clear();
endwin();

return 1;
}
