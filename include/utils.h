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
  enum : unsigned char {noop=0, self, cleanup, command1, command2, command3, END }; 
}

using uchar = unsigned char;
