#include "include/layers/packets/dlink_packet.h"
#include "include/layers/dlink_layer.h"
#include <chrono>
#include <ranges>
#include <algorithm>
#include <utility>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////.............DataLinkPackets  methods..........////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DatalinkPacket::DatalinkPacket(typename BasePacket::type base )
: BasePacket( base )
{

}

DatalinkPacket::DatalinkPacket( ushort op )
: BasePacket( std::make_shared<base_pipeline_data>() )
{
  set_op( op );
}

std::optional<unsigned char * >
DatalinkPacket::try_extract_dev_info()
{

  return {};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////.............device information methods..........//////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DatalinkPacket::device_information::device_information(){}

DatalinkPacket::device_information::device_information( const DatalinkPacket::device_information& rhs)
{
  *this = rhs;
}

DatalinkPacket::device_information 
DatalinkPacket::device_information::deserialize( unsigned char * raw_data )
{
  return device_information{};
}

void DatalinkPacket::device_information::stamp_time( )
{
  _timestamp = std::chrono::system_clock::now();
}

void DatalinkPacket::device_information::set_timeout( ulong timeout )
{
  _timeout = timeout;
}


bool DatalinkPacket::device_information::update_still_alive( )
{
  auto mark = std::chrono::system_clock::now();
  auto elapsed_time = (mark - _timestamp );
  auto seconds_elapsed = std::chrono::duration_cast<std::chrono::seconds>(elapsed_time);

  auto is_device_active = is_active();

  if ( seconds_elapsed.count() > _timeout ) 
  {
    if( !is_device_active ) state_has_changed();
    else state_hasnt_changed();

    activate();
  }
  else 
  {

    if( is_device_active ) state_has_changed();
    else state_hasnt_changed();

    deactivate();
  }
  return true; 
 
}
bool DatalinkPacket::device_information::mac_exists( std::string mac)
{
  bool found = std::ranges::any_of(_macs, [&](auto cand_mac)
  {
    return mac == cand_mac.second;
  } );
 
  return found;
}

std::pair<size_t, unsigned char *>
DatalinkPacket::device_information::get_extra() const
{

  
  return {0, nullptr};
}


DatalinkPacket::device_information&
DatalinkPacket::device_information::operator=(const DatalinkPacket::device_information& rhs )
{
  _timestamp = rhs._timestamp;
  _descs = rhs._descs;
  _active = rhs._active;
  _timeout = rhs._timeout;
  _state_chg = rhs._state_chg;

  //append mac address
  std::ranges::for_each(rhs._macs,[&](auto mac)
  {
    if( !mac_exists( mac.second ) ){
      auto element = 
        std::make_pair<bool, std::string>( true, std::move(mac.second));

      _macs.insert(_macs.end(), element );
    }
    
  } );

  return *this; 
}

bool operator==( const DatalinkPacket::device_information& lhs,
                 const DatalinkPacket::device_information& rhs )
{
  return (lhs._descs == rhs._descs) && (lhs._macs == rhs._macs );

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////.............Shared memory methods..........///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool DLinkSM::device_exists( const DatalinkPacket::device_information& cand_dev) const
{

  //go through every device
  bool found = std::ranges::any_of( _devices.second, [&](auto device)
  {
    //go through each mac in the device
    return std::ranges::any_of(device._macs, [&](auto mac)
    {
      //go through each mac of the device and compare to list
      return std::ranges::any_of(cand_dev._macs, [&](auto cand_mac)
      {
        return mac.second == cand_mac.second;

      } );

    }); 

  });

  return found;
}

DatalinkPacket::device_information& 
DLinkSM::get_device_info( const DatalinkPacket::device_information& dev_info )
{
  //search for just mac
  auto cand_flow = _devices.second | std::views::filter(
  [&](auto device )
  {
    return std::ranges::all_of( device._macs, [&](auto mac)
    {
      return std::ranges::all_of( dev_info._macs, [&](auto cand_mac)
      {
        return mac.second == cand_mac.second;

      } );
      
    } );
  } ) | std::views::take(1);
  
  return *std::ranges::find_if(cand_flow, []( auto ){ return true; } ); 
  
}

bool DLinkSM::exact_match(const DatalinkPacket::device_information& cand_dev ) const
{
  auto deviceIt = std::ranges::find( _devices.second, cand_dev );

  return (deviceIt != std::end(_devices.second) );
}



