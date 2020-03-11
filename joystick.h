#include <iostream>

#include <fcntl.h>

#include <fstream>

#include <array>

#include <iterator>

#include <map>

#include <ncurses.h>

#include <xdo.h>

#define JS_EVENT_BUTTON 0x01

#define JS_EVENT_AXIS 0x02

#define JS_EVENT_INIT 0x80

struct js_event
{
  uint32_t time;
  short value;
  uint8_t type;
  uint8_t number;
};

bool write_map_to_file ( std::map < std::array < int , 3 > , int > keybindings , bool append_flag = 0 );

std::array < int , 3 > read_button_press ( int fd , bool output_enable = 0 , bool report_button_release = 0 );

bool set_key_binding ( std::map < std::array < int , 3 > , int >& keybindings , int fd );
