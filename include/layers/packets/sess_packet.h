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

  struct ctrl_intf
  {
    friend class SessionPacket;

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
  SessionPacket( typename BasePacket::type base );

  SessionPacket( ushort );

  std::optional<unsigned char *>
  try_extract_dev_info();

  enum : unsigned char { discovery=PresentationPacket::END+1, device_info, END };
};

struct SessionPacket::device_information : public base_device_information
{
  static device_information deserialize( unsigned char *);

  device_information& operator =( const device_information& );

  std::string get_id() const  { return _id; }

  std::string _id;
};

bool operator==( const SessionPacket::device_information&,
                 const SessionPacket::device_information& );

