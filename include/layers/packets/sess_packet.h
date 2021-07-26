#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/pres_packet.h"

#pragma once

struct SessionPacket : public BasePacket
{
  using BasePacket::get_base;

  struct device_information;

   //restriction interface view
  SessionPacket( typename BasePacket::type base );

  SessionPacket( ushort );

  enum : unsigned char { discovery=PresentationPacket::END+1, device_info, deactivate_device, END };
};

struct SessionPacket::device_information : public base_device_information
{
  static device_information deserialize( const SessionPacket& );

  device_information( std::string id, ushort alc, ushort dec_occ, ushort dev_reprog) 
  : _id(id), _avg_link_congestion(alc), _device_occupancy( dec_occ ), _device_reprog( dev_reprog ) {}
 
  std::string get_id() const  { return _id; }

  void activate()   { _active = true;  }
  void deactivate() { _active = false; }

  void set_id( std::string id) { _id = id; }
  void set_alcong( ushort congestion ) { _avg_link_congestion = congestion; }
  void set_dev_ocp(ushort occupancy) { _device_occupancy = occupancy; } 
  void set_dev_reprog( ushort reprog_cost ) { _device_reprog = reprog_cost; } 


  bool _active=true;
  std::string _id;
  ushort _avg_link_congestion;
  ushort _device_occupancy;
  ushort _device_reprog;
 
};

