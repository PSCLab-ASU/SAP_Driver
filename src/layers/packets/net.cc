#include "include/layers/packets/net_packet.h"
#include "include/layers/net_layer.h"
#include <numeric>

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Network Packet device info......////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkPacket::device_information::device_information()
{

}

NetworkPacket::device_information::device_information( const std::vector<std::string>& aux_macs )
{
  std::ranges::copy(aux_macs, std::back_inserter(_macs) ); 
}

NetworkPacket::device_information::device_information( const NetworkPacket::device_information& rhs)
{

  *this = rhs; 
}

NetworkPacket::device_information& 
NetworkPacket::device_information::update(const NetworkPacket::device_information& rhs)
{
  //only update link status, congestion and mac
  //but mac_addr should be the same
  _macs[0] = rhs.get_mparms()[0]; 
  //Update base information
  set_desc ( rhs.get_desc() );
  //update active
  if ( rhs.is_active() )  activate();
  
  return *this;
}

size_t NetworkPacket::device_information::get_average_congestion() const
{
  size_t total = std::accumulate(std::begin(get_mparms()),
                                 std::end( get_mparms() ),
                                 (size_t) 0,
                                 [](size_t sum, auto mparm)
  {
    return sum + mparm.link_cong;
  } );
 
  auto avg = ( total / get_mparms().size() ) * 100;  

  return avg;
                  
}


NetworkPacket::device_information 
NetworkPacket::device_information::deserialize( const NetworkPacket & np )
{
 
  std::vector<std::string> aux_macs;
  auto[mac_sz, n_macs, macs_ptr] = np.get_tlv(0);
 
  /*printf("NET FOUND_MACS  : %i, %i : %02x:%02x:%02x:%02x:%02x:%02x \n", mac_sz,  n_macs, 
         macs_ptr[0],macs_ptr[1],macs_ptr[2],macs_ptr[3],macs_ptr[4],macs_ptr[5] );

  printf("NET FOUND_MACS2  : %i, %i : %02x:%02x:%02x:%02x:%02x:%02x \n", mac_sz,  n_macs, 
         macs_ptr[6],macs_ptr[7],macs_ptr[8],macs_ptr[9],macs_ptr[10],macs_ptr[11] );
  */
  for( int i=0; i < n_macs; i++  ){

    std::string mac = std::string( (char *) &macs_ptr[6*i], 6 );
    printf("NET FOUND_MACS  : %i, %i : %02x:%02x:%02x:%02x:%02x:%02x \n", mac_sz,  n_macs, 
         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );

    aux_macs.push_back(mac); 
  }

  auto dev_info( aux_macs );

  return dev_info;
}

bool operator==( const NetworkPacket::device_information& lhs,
                 const NetworkPacket::device_information& rhs )
{
  return std::ranges::all_of(lhs.get_mparms(), [&](auto lhs_mparm)
  {
    return std::ranges::any_of(rhs.get_mparms(), [&](auto rhs_mparm)
    {
      return lhs_mparm.get_mac() == rhs_mparm.get_mac();

    } );

  } );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Shared memory methods.....//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool NetSM::device_exists( const NetworkPacket::device_information& cand_dev) const
{

  //go through every device
  return std::ranges::any_of( _devices.second, [&](auto device)
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

}

bool NetSM::exact_match(const NetworkPacket::device_information& cand_dev ) const
{
  auto deviceIt = std::ranges::find( _devices.second, cand_dev );

  return (deviceIt != std::end(_devices.second) );
}

NetworkPacket::device_information& 
NetSM::get_device_info( const NetworkPacket::device_information& cand_device )
{ 
  return *std::ranges::find_if(_devices.second, [&]( auto dev_info )
  { 
    return dev_info == cand_device; 
  } ); 
  
}

