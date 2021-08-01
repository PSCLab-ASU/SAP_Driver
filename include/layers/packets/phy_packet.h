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
    auto ctl = BasePacket::get_ctrl<false>();
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
  PhyPacket() : PhyPacket( common_layer_cmds::noop ){}  

   //restriction interface view
  PhyPacket( typename BasePacket::type base );

  void parse_ctrl(){
    auto data = get_data();
    auto& ctrl = BasePacket::get_ctrl<false>(); //<--- THIS IS SOME VOODOO
    auto ctrl_sz = data[0] + 1;
    printf(" parse_ctrl sz : %02x, op : %02x\n", ctrl_sz, data[ctrl_sz] );

    ctrl = std::vector<uchar>(ctrl_sz+2, 0);
    set_op( data[ctrl_sz] );

    //data[0] is size, and data[ctrl_sz] is the operation

    ctrl[1] = ctrl_sz-1;
    for(ulong i=1; i < ctrl_sz; i++) ctrl[i+2] = data[i];
    set_offsets(0, ctrl_sz);

  }

  enum : unsigned char { send=DatalinkPacket::END+1, get_mac, set_intfs, END };
};

