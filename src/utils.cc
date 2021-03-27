#include <ranges>
#include <thread>
#include <chrono>
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


#endif
