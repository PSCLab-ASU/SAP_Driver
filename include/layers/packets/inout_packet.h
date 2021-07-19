#include <queue>
#include <ranges>
#include <iostream>
#include <optional>
#include <algorithm>
#include "include/utils.h"
#include "include/pipeline_data.h"

#pragma once

/* Custom packet interface for each layer helper */
struct InOutPacket : public BasePacket
{
  using BasePacket::get_base;

  struct ctrl_intf
  {
    friend class InOutPacket;

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

  InOutPacket() : BasePacket(){}

   //restriction interface view
  InOutPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}
 
  enum : unsigned char { output=common_layer_cmds::END+1, END };
};

