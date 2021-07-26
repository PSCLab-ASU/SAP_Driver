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


   //restriction interface view
  NetworkPacket( typename BasePacket::type base );

  NetworkPacket( ushort op=common_layer_cmds::noop );

  enum : unsigned char { var=TransportPacket::END+1, keep_alive, deactivate_port, END };
};


struct NetworkPacket::device_information : public base_device_information
{
  struct mac_params{
    bool link_status=false;
    uint link_cong=0;
    std::string _mac_addr;

    bool get_link_status(){ return link_status; }
    void deactivate() { link_status = false; }

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

  void set_lcong( std::string, ushort link_cong );
 
  const std::vector<mac_params>&
  get_mparams() const { return _macs; } 

  ushort get_avg_congest () const;

  void deactivate_port( std::string );

  bool is_inaccessible();

  static device_information deserialize( const NetworkPacket& );

  std::string _id;
  
  std::vector< mac_params > _macs; 

};


bool operator==(const NetworkPacket::device_information&,
                const NetworkPacket::device_information& );
