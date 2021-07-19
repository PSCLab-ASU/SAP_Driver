#include <mutex>
#include <map>
#include <ranges>
#include <thread>
#include <chrono>
#include <type_traits>
#include <vector>
#include <iostream>
#include <functional>
#include <boost/algorithm/string.hpp>

#pragma once

#define MAX_MTU 1516
#define ETH_H_LEN 16

using namespace std::chrono_literals;

struct Stat {};

struct NullType {};


constexpr auto heart_beat();

constexpr auto rest();

namespace common_layer_cmds { 
  enum : unsigned char {noop=0, discovery, self, cleanup, command1, command2, command3, END }; 
}

const unsigned int g_accelerator_type = 0xab80;

using uchar = unsigned char;


enum accel_desc_param { HW_VID=0, 
                        HW_PID, 
                        HW_SS_VID, 
                        HW_SS_PID, 
                        SW_VID, 
                        SW_PID, 
                        SW_CLID, SW_FID, SW_VERID, ACCEL_ID_END };

using accel_descriptor = std::map<accel_desc_param, uint>;

template<typename T>
struct equal_to
{
  equal_to(T eq_val) : val(eq_val){}

  bool operator()(T comp_val) 
  {
    return (val == comp_val);
  }

  T val;
};

struct base_device_information
{
 
  void set_param( accel_desc_param param, uint val){
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

  bool is_active() const {
   return _active;
  }

  const accel_descriptor&
  get_desc() const { return _descs; }

  void set_desc( const accel_descriptor& descs )
  {
    _descs = descs;
  }

  void update_desc( const accel_descriptor& descs )
  {
    for(auto& [key, val] : descs )
    {
      _descs[key] = val; 
    }
  }

  bool _active;
  accel_descriptor _descs;
  std::vector<unsigned char> _extra;
};
