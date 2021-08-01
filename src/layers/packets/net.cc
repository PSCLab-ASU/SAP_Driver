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
  _macs[0] = rhs.get_mparams()[0]; 
  //Update base information
  set_desc ( rhs.get_desc() );
  //update active
  if ( rhs.is_active() )  activate();
  
  return *this;
}

void NetworkPacket::device_information::set_lcong( std::string mac, ushort lcong)
{
  auto mparm = std::ranges::find_if(_macs, equal_to{ mac }, &mac_params::get_mac );
  mparm->link_cong = lcong;
}

ushort NetworkPacket::device_information::get_avg_congest() const
{
  size_t total = std::accumulate(std::begin(get_mparams()),
                                 std::end( get_mparams() ),
                                 (size_t) 0,
                                 [](size_t sum, auto mparm)
  {
    return sum + mparm.link_cong;
  } );
 
  auto avg = ( total / get_mparams().size() ) * 100;  

  return avg;
                  
}


NetworkPacket::device_information 
NetworkPacket::device_information::deserialize( const NetworkPacket & np )
{
 
  std::vector<std::string> aux_macs;
  auto[ n_macs, mac_sz, macs_ptr] = np.get_tlv(0);
 
  for( int i=0; i < n_macs; i++  ){

    std::string mac = std::string( (char *) &macs_ptr[6*i], 6 );
    printf("NET FOUND_MACS  : %i, %i : %02x:%02x:%02x:%02x:%02x:%02x \n", mac_sz,  n_macs, 
         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] ); 

    aux_macs.push_back(mac); 
  }

  auto dev_info( aux_macs );

  return dev_info;
}

void 
NetworkPacket::device_information::deactivate_port( std::string mac )
{
  auto devIt = std::ranges::find_if(_macs, equal_to{mac}, &mac_params::get_mac );
 
  if ( devIt != _macs.end() ) devIt->deactivate();
  else printf(" NET Port doesn't exists \n" ); 

}

bool 
NetworkPacket::device_information::is_inaccessible( )
{
  return
  std::ranges::all_of(_macs, equal_to{false}, 
                      &mac_params::get_link_status );
}

bool operator==( const NetworkPacket::device_information& lhs,
                 const NetworkPacket::device_information& rhs )
{
  return std::ranges::all_of(lhs.get_mparams(), [&](auto lhs_mparm)
  {
    return std::ranges::any_of(rhs.get_mparams(), [&](auto rhs_mparm)
    {
      auto mac = lhs_mparm.get_mac();
      auto cand_mac = rhs_mparm.get_mac();

      printf("NET CMP %02x:%02x:%02x:%02x:%02x:%02x == %02x:%02x:%02x:%02x:%02x:%02x \n",
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
              cand_mac[0], cand_mac[1], cand_mac[2], cand_mac[3], cand_mac[4], cand_mac[5]);
               
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
    return std::ranges::all_of(device._macs, [&](auto mac)
    {
      //go through each mac of the device and compare to list
      return std::ranges::any_of(cand_dev._macs, [&](auto cand_mac)
      {
        return mac == cand_mac;

      } );

    }); 

  });

}

bool NetSM::device_exists( std::string mac ) const
{

  //go through every device
  return std::ranges::any_of( _devices.second, [&](auto device)
  {
      //go through each mac of the device and compare to list
    return std::ranges::any_of(device._macs, equal_to{mac}, 
                               &NetworkPacket::device_information::mac_params::get_mac);

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
  for(auto& dev : _devices.second)
    if( device_exists(cand_device) )
      return dev;

  throw std::out_of_range("NET No devices found!");

  return *_devices.second.end();
}

NetworkPacket::device_information& 
NetSM::get_device_info( const std::string& src_mac, unsigned char& dev_index )
{ 
  dev_index = 0;
  printf("NET CMP : %02x:%02x:%02x:%02x:%02x:%02x \n",
         src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5] );

  for(auto& dev : _devices.second)
  {
    bool exists = std::ranges::any_of( dev.get_mparams(), 
                                       equal_to{ src_mac },
                                       &NetworkPacket::device_information::mac_params::get_mac );

    std::ranges::for_each(dev.get_mparams(),
      [](auto src_mac ){
        printf("NET CMP2 : %02x:%02x:%02x:%02x:%02x:%02x \n",
                src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5] );

      }, &NetworkPacket::device_information::mac_params::get_mac);
    
    if( exists ) return dev;
    dev_index += 1;
  }
  
  throw std::out_of_range("NET No devices found for src_mac");

  return *_devices.second.end();
}
