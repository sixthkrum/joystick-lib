#ifndef _XJS_H_
#define _XJS_H_

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

/*
struct to read joystick events from device
*/

struct js_event
{
  uint32_t time;
  short value;
  uint8_t type;
  uint8_t number;
};

/*
returns configuration folder path where map.bin is stored
*/

std::string config_path()
{
  std::string path;
  path = getenv ( "HOME" );
  path = path + "/.xjsconfig/";

  return path;
}

#define MAP_PATH config_path()

/*
writes the configured map to file, $HOME/.xjsconfig/map.bin

std::map < std::array < int , 3 > , std::string > keybindings : std::map that maps buttons to keysyms
bool append_flag : true to add to existing map.bin file, false to make a new map.bin map_file

returns true if file is written to successfully
        false otherwise
*/

bool write_map_to_file ( std::map < std::array < int , 3 > , std::string > &keybindings , bool append_flag = 0 );

/*
reads the configured map from file, $HOME/.xjsconfig/map.bin

std::map < std::array < int , 3 > , std::string > &keybindings : std::map that maps buttons to keysyms or keysym sequences seperated by +

returns true if file id read from successfully
        false otherwise
*/

bool read_map_from_file ( std::map < std::array < int , 3 > , std::string > &keybindings );

/*
returns first valid button press or release from joystick device

int fd : file descriptor of joystick device you want to read from
timeval *block_time : time to block while waiting for input, NULL to block until input is available
bool empty_device : true to empty all available events that can be read from device before blocking false otherwise
bool report_button_release : true to report button release instead of press false to report press
bool output_enable : true to output button pressed etc , false otherwise

returns { -1 , 0 , 0 } if joystick is not available
        { -2 , 0 , 0 } if no valid joystick event or no event happens while blocked
        { js_event.type , js_event.number , js_event.value } otherwise
*/

std::array < int , 3 > read_button_press ( int fd , timeval *block_time , bool empty_device = 1 , bool report_button_release = 0 , bool output_enable = 0 );

/*
binds button presses or releases to key press sequences using keysyms, seperate multiple X11 keysyms by +

int fd : file descriptor of joystick device you want to read from
std::map < std::array < int , 3 > , std::string > &keybindings : std::map that maps buttons to keysyms or keysym sequences seperated by +
bool report_button_release : true to report button release instead of press false to report press

returns false if binding is unsuccessful
        true if successfull
*/

bool set_key_binding ( int fd , std::map < std::array < int , 3 > , std::string > &keybindings , bool report_button_release = 0 );

/*
empties the given joystick device by reading all available events

int fd : file descriptor of the joystick device you want to empty
*/

void empty_joystick_device ( int fd );

/*
tries to simulate the mapped key sequences, if they are invalid it won't do anything

const xdo_t *x : main context
Window window : window to send key sequences to
(see in xdo.h documentation at https://github.com/jordansissel/xdotool/blob/master/xdo.h)
std::array < int , 3 > button : { js_event.type , js_event.number , js_event.value } entry in map to check binding to
std::map < std::array < int , 3 > , std::string > &keybindings : std::map that maps buttons to keysyms or keysym sequences seperated by +
bool output_enable : true to enable output false otherwise

returns value returned by xdo_send_keysequence_window in functionwhile( e.type != JS_EVENT_AXIS && e.type != JS_EVENT_BUTTON && retval != 0 )
*/

int simulate_mapped_key ( const xdo_t *x , Window window , std::array < int , 3 > button , std::map < std::array < int , 3 > , std::string > &keybindings , bool output_enable = 0 );

#endif
