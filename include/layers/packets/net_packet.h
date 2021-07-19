#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/trans_packet.h"

#pragma once

struct NetworkPacket : public BasePacket
{
  using BasePacket::get_base;

  struct device_information;

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

  NetworkPacket( ushort op=common_layer_cmds::noop );

  enum : unsigned char { var=TransportPacket::END+1, keep_alive, END };
};


struct NetworkPacket::device_information : public base_device_information
{
  struct mac_params{
    bool link_status=false;
    uint link_cong=0;
    std::string _mac_addr;
    mac_params( std::string mac ) : _mac_addr( mac) {}

    std::string get_mac() const { return _mac_addr; }

    bool operator ==(const mac_params& lhs){
      return (get_mac() == lhs.get_mac());
    }

  };

  device_information( );

  device_information( const device_information&);

  device_information( const std::vector<std::string>& );

  device_information& update(const device_information& );
 
  std::string get_id() const {
    return _id;
  }
   
  void set_id( size_t id ) {
    _id = std::to_string( id );
  }
 
  const std::vector<mac_params>&
  get_mparms() const { return _macs; } 

  size_t get_average_congestion () const;

  static device_information deserialize( const NetworkPacket& );

  std::string _id;
  
  std::vector< mac_params > _macs; 

};


bool operator==(const NetworkPacket::device_information&,
                const NetworkPacket::device_information& );
