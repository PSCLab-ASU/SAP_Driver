#include <mutex>
#include <map>
#include <ranges>
#include <thread>
#include <chrono>
#include <type_traits>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>

#pragma once

#define MAX_MTU 1500

using namespace std::chrono_literals;

struct Stat {};

struct NullType {};


constexpr auto heart_beat();

constexpr auto rest();

namespace common_layer_cmds { 
  enum : unsigned char {noop=0, discovery, self, cleanup, command1, command2, command3, END }; 
}

const std::string g_accelerator_type = { (char) 0x80, (char) 0xAB, (char) 0 };

using uchar = unsigned char;

enum accel_desc_param { HW_ID=0, 
                        HW_PID, 
                        HW_SS_VID, 
                        HW_SS_PID, 
                        SW_VID, 
                        SW_PID, 
                        SW_CLID, SW_FID, SW_VERID, ACCEL_ID_END };


struct base_device_information
{
 
  void set_param( accel_desc_param param, int val){
    _descs[param] = val;
  }

  std::pair<size_t, const unsigned char *>
  get_extra() const
  {
    return std::make_pair( _extra.size(), _extra.data() );
  }

  void set_extra(size_t nbyte, unsigned char * data)
  {
    for(size_t i=0; i < nbyte; i++) _extra.push_back( data[i] );
  }

  std::vector<unsigned char> serialize_desc() const;

  void activate()   { _active = true; }
  void deactivate() { _active = false; }

  bool is_active(){
   return _active;
  }

  bool _active;
  std::map<accel_desc_param, uint> _descs;
  std::vector<unsigned char> _extra;
};
