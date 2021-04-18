#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/trans_packet.h"

#pragma once

struct NetworkPacket : public BasePacket
{
  using BasePacket::get_base;

  struct ctrl_intf
  {
    friend class NetworkPacket;

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
  NetworkPacket( typename BasePacket::type base );

  enum : unsigned char { var=TransportPacket::END+1, END };
};

