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
    
      auto& get(){ return _ctrl; }
  
      void append_data( ushort val )
      {
        std::cout << " Appending ctrl data : " << val << std::endl;
        _ctrl.push_back(val);
      }

    private:

      ctrl_intf( const std::vector<uchar>& ctrl )
      : _ctrl( ctrl ) 
      {}     
 
      ctrl_intf( ){}

      std::vector<uchar> _ctrl;  
    
  };


  ctrl_intf get_ctrl()
  {
    auto ctl = BasePacket::get_ctrl();
    return ctl;
  }

  auto get_ctrl_data()
  {
    auto ctl = get_ctrl();
    auto data = ctl.get();
    
    //incremented iterator once to bypass the op code
    //being saved in the first slot       
    return std::vector<uchar>( std::next( data.begin() ), data.end() );
  }

  static ctrl_intf create_ctrl( )
  {
    return ctrl_intf();
  }
   
  PhyPacket(ushort op );

   //restriction interface view
  PhyPacket( typename BasePacket::type base );

  enum : unsigned char { send=DatalinkPacket::END+1, get_mac, set_intfs, END };
};

