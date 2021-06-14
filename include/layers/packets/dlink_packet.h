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

  struct ctrl_intf
  {
    friend class DatalinkPacket;

    public :
    
      auto get(){ return _ctrl; }

    private:

      ctrl_intf( const std::vector<uchar>& ctrl )
      : _ctrl( ctrl ) 
      {}     
 
      ctrl_intf( ){}

      std::vector<uchar> _ctrl;  
    
  };

  std::optional<unsigned char * > 
  try_extract_dev_info();

  ctrl_intf get_ctrl()
  {
    return ctrl_intf( get_ctrl() );
  }

  static ctrl_intf create_ctrl( )
  {
    return ctrl_intf();
  }

  DatalinkPacket( );

  DatalinkPacket(ushort op );

   //restriction interface view
  DatalinkPacket( typename BasePacket::type base );

  enum : unsigned char { set_mac=NetworkPacket::END+1, END };
};

struct DatalinkPacket::device_information : public base_device_information
{

  device_information( );

  device_information( const device_information& );

  device_information& operator =(const device_information& rhs );

  static device_information deserialize( unsigned char * );
 
  //sets timestamp to right now 
  void stamp_time();

  //sets the delta time before device is lost
  void set_timeout( ulong );

  //update the device on whether it is active or lost
  bool update_still_alive();

  bool mac_exists( std::string mac );

  std::pair<size_t, unsigned char *>
  get_extra() const;

  bool has_state_changed(){
    bool sc = _state_chg;
    _state_chg = false;

    return sc;
  }

  void state_has_changed(){
    _state_chg = true;
  }

  void state_hasnt_changed(){
    _state_chg = false;
  }

  void lock(){
    _mu.lock();
  }

  void unlock(){
    _mu.unlock();
  }
 
  std::mutex _mu; 
  ulong _timeout;
  bool _state_chg;
  std::vector< std::pair<bool, std::string> > _macs;
  std::chrono::time_point<std::chrono::system_clock> _timestamp;
  
};

bool operator==(const DatalinkPacket::device_information&,
                const DatalinkPacket::device_information& );

