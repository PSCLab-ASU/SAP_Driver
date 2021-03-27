#include <ranges>
#include <thread>
#include <chrono>
#include <type_traits>


#pragma once

using namespace std::chrono_literals;

struct Stat
{

};

struct NullType 
{


};

constexpr auto heart_beat();

constexpr auto rest();

namespace common_layer_cmds { 
  enum : unsigned char {self=0, command1, command2, command3, command4 }; 
}

using uchar = unsigned char;
