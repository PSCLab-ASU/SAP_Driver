#include <iostream>
#include <ranges>
#include <chrono>
#include <mutex>
#include "include/utils.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/net_packet.h"

#pragma once


struct DatalinkPacket : public BasePacket
{
  using BasePacket::get_base;

  struct device_information;

  DatalinkPacket(ushort op = common_layer_cmds::noop );

   //restriction interface view
  DatalinkPacket( typename BasePacket::type base );

  //get the interface this packet came from
  //std::string get_intf_name() const;
  //this method extracts the mac address from the sending port
  //std::string get_src_mac() const;
  //this methods extracts the macs associated with the sending port
  std::vector<std::string> get_aux_macs() const;

  enum : unsigned char { set_mac=NetworkPacket::END+1, END };
};

struct DatalinkPacket::device_information : public base_device_information
{

  struct mac_params{
    bool link_status;
    volatile bool last_link_status;
    std::string intf_name;
    std::string src_mac;
    std::chrono::time_point<std::chrono::system_clock> _timestamp;
    mac_params( bool lstat, std::string intf, std::string mac )
    { link_status=lstat; intf_name=intf; src_mac=mac; }
    /////////////////////////funcs/////////////////////////////////////////
    void set_keepalive() { _timestamp = std::chrono::system_clock::now(); }
    auto get_keepalive() { return _timestamp; }
    void expired(){ last_link_status = link_status; link_status = false; }
    void renew(){ last_link_status = link_status; link_status = true; }
  };


  device_information( );

  device_information(accel_descriptor, std::string, std::string, std::vector<std::string>, bool );

  device_information( const device_information& );

  device_information& operator =(const device_information& rhs );

  static device_information deserialize( DatalinkPacket&, std::string&, bool adv_tlv=true );
 
  //sets the delta time before device is lost
  void set_timeout( ulong );

  //update the device on whether it is active or lost
  bool assess_expiration( );

  //get recently expired macs since last call to get_recently_exp_macs
  std::vector<std::string> get_recently_exp_macs();

  void update_link_status( int, std::string, bool );

  bool mac_exists( std::string mac ) const;

  std::string serialize_macs() const;

  size_t get_nports() const
  {
    return _macs.size(); 
  }
 
  void lock(){
    _mu.lock();
  }

  void unlock(){
    _mu.unlock();
  }

  bool is_device_accessible() const
  {
    return _still_alive;
  } 

  ulong get_timeout() const { return _timeout; }
 
  const std::vector<mac_params> & get_macs() const { return _macs; }
  size_t get_nmacs() const { return _macs.size(); }

  

  std::mutex _mu; 
  bool _partial_info;
  bool _still_alive;
  ulong _timeout;
  std::vector< mac_params > _macs;
  
};

