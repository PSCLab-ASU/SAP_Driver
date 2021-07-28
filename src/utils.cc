#include <ranges>
#include <thread>
#include <chrono>
#include <sstream>
#include <iostream>
#include <numeric>
#include <functional>
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

base_device_information 
base_device_information::deserialize( const uchar * desc_ptr)
{
  base_device_information bdi;
  const uint * desc = (uint *) desc_ptr;

  for(int i=0; i < ACCEL_ID_END; i++)
  {
    bdi.set_param((accel_desc_param)i, desc[i] );
  }

  return bdi;
}

std::string base_device_information::stringify_desc() const
{
  auto colon_fold = [](std::string a, auto b) 
  {
    return std::move(a) + ':' + std::to_string(b.second);
  };
 
  std::string first = std::to_string(_descs.at((accel_desc_param)0 ));
  std::string sdesc = std::accumulate(std::next(_descs.begin()), _descs.end(),
                                      first, colon_fold);

  return sdesc;
}
#endif
