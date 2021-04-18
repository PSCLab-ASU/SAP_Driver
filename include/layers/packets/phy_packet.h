#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/dlink_packet.h"

#pragma once

struct PhyPacket : public BasePacket
{
  using BasePacket::get_base;

  struct ctrl_intf
  {
    friend class PhyPacket;

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
   
  PhyPacket(ushort op );

   //restriction interface view
  PhyPacket( typename BasePacket::type base );

  enum : unsigned char { send=DatalinkPacket::END+1, get_mac, END };
};

