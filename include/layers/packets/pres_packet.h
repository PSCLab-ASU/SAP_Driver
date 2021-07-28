#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/inout_packet.h"

#pragma once

struct PresentationPacket : public BasePacket
{
  using BasePacket::get_base;

  //restriction interface view
  PresentationPacket( typename BasePacket::type base );

  PresentationPacket( ushort );  

  enum : unsigned char { var=InOutPacket::END+1, get_devices, END };

};

