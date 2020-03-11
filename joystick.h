#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <iostream>

#include <fcntl.h>

#include <fstream>

#include <map>

#include <array>

#include <string>

#include <iterator>

extern "C"
{
  #include <xdo.h>
}


#define JS_EVENT_BUTTON 0x01

#define JS_EVENT_AXIS 0x02

#define JS_EVENT_INIT 0x80

#define TRUE 1

#define FALSE 0

#define END_OF_ENTRY 0

struct js_event
{
  uint32_t time;
  short value;
  uint8_t type;
  uint8_t number;
};

bool write_map_to_file ( std::map < std::array < int , 3 > , std::string > keybindings , bool append_flag = 0 );

bool read_map_from_file ( std::map < std::array < int , 3 > , std::string >& keybindings );

std::array < int , 3 > read_button_press ( int fd , unsigned int block_time_usec = 0 , bool output_enable = 0 , bool report_button_release = 0 );

bool set_key_binding ( std::map < std::array < int , 3 > , std::string >& keybindings , int fd );

#endif
