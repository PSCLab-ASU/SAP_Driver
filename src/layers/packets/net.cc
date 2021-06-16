#include "include/layers/packets/net_packet.h"
#include "include/layers/net_layer.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Network Packet Interface......//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkPacket::NetworkPacket(typename BasePacket::type base )
: BasePacket (base)
{


}

NetworkPacket::NetworkPacket( ushort op )
: BasePacket( std::make_shared<base_pipeline_data>() )
{
  set_op( op );
}

std::optional<unsigned char * >
NetworkPacket::try_extract_dev_info()
{

  return {};
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Network Packet device info......////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkPacket::device_information::device_information()
{

}

NetworkPacket::device_information::device_information( const NetworkPacket::device_information&)
{

}

NetworkPacket::device_information& 
NetworkPacket::device_information::operator =(const NetworkPacket::device_information& rhs)
{

  return *this;
}

NetworkPacket::device_information 
NetworkPacket::device_information::deserialize( unsigned char *)
{
  return device_information{};
}

bool operator==( const NetworkPacket::device_information& lhs,
                 const NetworkPacket::device_information& rhs )
{
  return ( lhs.get_id() == rhs.get_id() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Shared memory methods.....//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool NetSM::device_exists( const NetworkPacket::device_information& cand_dev) const
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
        return mac == cand_mac;

      } );

    }); 

  });

  return found;
}

bool NetSM::exact_match(const NetworkPacket::device_information& cand_dev ) const
{
  auto deviceIt = std::ranges::find( _devices.second, cand_dev );

  return (deviceIt != std::end(_devices.second) );
}

NetworkPacket::device_information& 
NetSM::get_device_info( const NetworkPacket::device_information& dev_info )
{
  //search for just mac
  auto cand_flow = _devices.second | std::views::filter(
  [&](auto device )
  {
    return std::ranges::all_of( device._macs, [&](auto mac)
    {
      return std::ranges::all_of( dev_info._macs, [&](auto cand_mac)
      {
        return mac == cand_mac;

      } );
      
    } );
  } ) | std::views::take(1);
  
  return *std::ranges::find_if(cand_flow, []( auto ){ return true; } ); 
  
}

