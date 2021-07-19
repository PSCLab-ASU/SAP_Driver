#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/sess_packet.h"

#pragma once

struct TransportPacket : public BasePacket
{
  using BasePacket::get_base;

  struct device_information;

  struct ctrl_intf
  {
    friend class TransportPacket;

    public :
    
      auto get(){ return _ctrl; }

    private:

      ctrl_intf( const std::vector<uchar>& ctrl )
      : _ctrl( ctrl ) 
      {}     
 
      ctrl_intf( ){}

      std::vector<uchar> _ctrl;  
    
  };

  ctrl_intf get_ctrl()
  {
    return ctrl_intf( get_ctrl() );
  }

  static ctrl_intf create_ctrl( )
  {
    return ctrl_intf();
  }

   //restriction interface view
  TransportPacket( typename BasePacket::type base );

  TransportPacket( ushort );

  std::optional<unsigned char *>
  try_extract_dev_info();

  enum : unsigned char { var=SessionPacket::END+1, END };
};


struct TransportPacket::device_information : public base_device_information
{
  static device_information deserialize( unsigned char *);

  device_information();

  device_information( const device_information & );

  device_information& operator=(const device_information& );

  std::string get_id() const { return _id; }

  std::string _id;
  bool _active;
  uint _avg_link_congestion; //0 - 100/2^32
  uint _device_occupancy;    //0 - 100/2^32
  uint _reprog_penalty;      //0 - 2^32 (in ns)

};
