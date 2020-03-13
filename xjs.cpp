#include "joystick.h"

void empty_joystick_device ( int fd )
{
  fd_set rfds;
  timeval tv;
  int retval;
  js_event e;

  do
  {
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO ( &rfds );
    FD_SET ( fd , &rfds );

    retval = select ( fd + 1 , &rfds , NULL , NULL , &tv );

    if ( FD_ISSET(fd,&rfds) )
    {
      read ( fd , &e , sizeof ( e ) );
    }

  } while( retval > 0 );

}

bool write_map_to_file ( std::map < std::array < int , 3 > , std::string > &keybindings , bool append_flag )
{
  std::fstream map_file;

  if ( append_flag )
  {
    map_file.open ( MAP_PATH + "map.bin" , std::ios::binary | std::ios::out | std::ios::app );
  }

  else
  {
    remove ( ( MAP_PATH + "map.bin" ).c_str() );
    map_file.open ( MAP_PATH + "map.bin" , std::ios::binary | std::ios::out );
  }

  if ( map_file.bad() )
  {
    return 0;
  }

  std::map < std::array < int , 3 > , std::string > :: iterator i;
  char end_of_entry_flag = END_OF_ENTRY;

  for ( i = keybindings.begin() ; i != keybindings.end() ; i ++ )
  {
    map_file.write ( ( char* ) & i -> first [0] , sizeof ( int ) );
    map_file.write ( ( char* ) & i -> first [1] , sizeof ( int ) );
    map_file.write ( ( char* ) & i -> first [2] , sizeof ( int ) );
    map_file.write ( ( char* ) & i -> second [0] , i -> second.length() );
    map_file.write ( ( char* ) & end_of_entry_flag , 1 );
  }

  if ( map_file.bad() )
  {
    map_file.close();
    return 0;
  }

  map_file.close();

  return 1;
}

bool read_map_from_file ( std::map < std::array < int , 3 > , std::string > &keybindings )
{
  std::fstream map_file;
  map_file.open ( MAP_PATH + "map.bin" , std::ios::binary | std::ios::in );

  if ( map_file.peek() == EOF || map_file.bad() )
  {
    return 0;
  }

  std::array < int , 3 > button;
  std::string key , temp;

  while ( map_file.peek() != EOF )
  {
    map_file.read ( ( char* ) & button [0] , sizeof ( int ) );
    map_file.read ( ( char* ) & button [1] , sizeof ( int ) );
    map_file.read ( ( char* ) & button [2] , sizeof ( int ) );

    map_file.read ( ( char* ) & temp [0] , 1 );
    key.clear();

    while ( char ( temp [0] ) != END_OF_ENTRY )
    {
      key = key + temp [0];
      map_file.read ( ( char* ) & temp [0] , 1 );
    }

    keybindings.insert ( std::pair < std::array < int , 3 > , std::string > ( button , key ) );
  }

  if ( map_file.bad() )
  {
    map_file.close();
    return 0;
  }

  map_file.close();

  return 1;
}

std::array < int , 3 > read_button_press ( int fd , timeval *block_time , bool empty_device , bool report_button_release , bool output_enable )
{
  if ( fd > 0 && output_enable )
  {
    std::cout << "joystick opened" << '\n';
  }

  else if ( fd <= 0 )
  {
    if ( output_enable )
    {
      std::cout << "no joystick available" << '\n';
      usleep ( 500000 );
    }

    return { -1 , 0 , 0 };
  }

  if ( empty_device )
  {
    empty_joystick_device ( fd );
  }

  fd_set rfds;

  if ( block_time != NULL )
  {
    FD_ZERO ( &rfds );
    FD_SET ( fd , &rfds );

    select ( fd + 1 , &rfds , NULL , NULL , block_time );
  }

  js_event e , temp;
  int retval;

  if ( block_time == NULL || FD_ISSET ( fd , &rfds ) )
  {

    read ( fd , &temp , sizeof ( js_event ) );

    while( e.type != JS_EVENT_AXIS && e.type != JS_EVENT_BUTTON && retval != 0 )
    {
      e = temp;
      retval = read ( fd , &temp , sizeof ( js_event ) );
    }

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

        if ( report_button_release ^ e.value )
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

        if ( report_button_release ^ e.value )
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

  if ( output_enable )
  {
    std::cout << "no valid event available" << std::endl;
  }

  return { -2 , 0 , 0 };
}

bool set_key_binding ( int fd , std::map < std::array < int , 3 > , std::string > &keybindings , bool report_button_release )
{

  if (fd > 0)
  {
    std::cout << "joystick opened" << std::endl;
  }

  else
  {
    std::cout << "no joystick available" << std::endl;
    usleep ( 500000 );

    return 0;
  }

  while ( TRUE )
  {
    std::cout << "enter X11 keysym to bind button to (seperate by + for multi): " << std::endl;

    std::string key;
    std::cin >> key;

    std::cout << "press button that will map to the key: " << std::endl;

    std::array < int , 3 > button;

    button = read_button_press ( fd , NULL , 1 , report_button_release );

    switch ( button [0] )
    {
      case -2:
      {
        std::cout << "error" << '\n';
        usleep ( 500000 );

        return 0;
      } break;

      case -1:
      {
        std::cout << "the joystick has been disconnected, exiting" << std::endl;
        usleep ( 500000 );

        return 0;
      } break;

      default:
      {
        std::cout << button [0] << '\t' << button [1] << '\t' << button [2];
        keybindings.insert ( std::pair < std::array < int , 3 > , std::string > ( button , key ) );
      } break;

    }

    std::cout << '\n' << "bind another key? enter y to continue" << std::endl;

    char choice;
    std::cin >> choice;

    if ( choice == 'y' || choice == 'Y' )
    {
      system ( "clear" );
      continue;
    }

    else
    {
      break;
    }

  }

return 1;
}

int simulate_mapped_key ( const xdo_t *x , Window window , std::array < int , 3 > button , std::map < std::array < int , 3 > , std::string > &keybindings , bool output_enable )
{
  std::string key;
  int retval;

  try
  {
    key = keybindings.at ( button );

    if ( output_enable )
    {
      std::cout << key << std::endl;
    }

    retval = xdo_send_keysequence_window ( x , window , key.c_str() , 0 );
  }

  catch ( std::out_of_range )
  {
    if ( output_enable )
    {
      std::cout << "nothing mapped to this button" << std::endl;
    }

  }

  return retval;

}
