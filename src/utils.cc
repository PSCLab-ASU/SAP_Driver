#include <ranges>
#include <thread>
#include <chrono>
#include <sstream>
#include <iostream>
#include "include/utils.h"

#ifndef UTILS
#define UTILS

using namespace std::chrono_literals;

constexpr auto rest()
{
  return std::ranges::views::transform([](auto in)
         { 
           std::this_thread::sleep_for( 100ms );  
           return std::forward<decltype(in)>(in); 
         });
}

constexpr auto heart_beat()
{
  return std::ranges::views::iota(0) | rest();
}

std::vector<unsigned char>
base_device_information::serialize_desc() const
{
  std::vector<uchar> out;
  for(int i=0; i < ACCEL_ID_END; i++)
  {
    const uchar * val = (const unsigned char *) &_descs.at( (accel_desc_param)i);

    for(int j=0; j < sizeof(uint); j++) out.push_back( val[j] );
  }

  return out;   
}

#endif
