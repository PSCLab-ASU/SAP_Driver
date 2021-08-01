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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////.............device information methods..........//////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DatalinkPacket::device_information::device_information(){}

DatalinkPacket::device_information::device_information(accel_descriptor, 
                                                       std::string intf_name, 
                                                       std::string src_mac, 
                                                       std::vector<std::string> aux_macs,
                                                       bool partial_device_info = true)
{
  _partial_info = partial_device_info;

  std::ranges::transform(aux_macs, std::back_inserter(_macs),  [&]( auto aux_mac ) 
  {
    bool this_link   = (aux_mac == src_mac);
    std::string intf = this_link?intf_name:"";
    
    mac_params mcp( this_link, intf, aux_mac);
    if(this_link) mcp.set_keepalive();

    return mcp;
  } );

}

DatalinkPacket::device_information::device_information( const DatalinkPacket::device_information& rhs)
{
  *this = rhs;
}

DatalinkPacket::device_information 
DatalinkPacket::device_information::deserialize( DatalinkPacket& dp, std::string& src_mac, bool adv_tlv )
{
  std::vector<std::string> aux_macs;
  auto intf_name   = std::to_string(dp.get_intf_id() );
  auto _src_mac    = dp.get_src();
  accel_descriptor desc; 

  src_mac = _src_mac;
 
  printf("FOUND_SRC : %02x:%02x:%02x:%02x:%02x:%02x \n",
          ( 0xFF & _src_mac.c_str()[0]),
          ( 0xFF & _src_mac.c_str()[1]),
          ( 0xFF & _src_mac.c_str()[2]),
          ( 0xFF & _src_mac.c_str()[3]),
          ( 0xFF & _src_mac.c_str()[4]),
          ( 0xFF & _src_mac.c_str()[5]));

  //TBD: Add Accelerator description vector
  //6B   n_macs ptr
  auto ctrl_vec = dp.get_ctrl<false>();

  std::cout << "FOUND_CTRL : ";
  std::ranges::copy(ctrl_vec, std::ostream_iterator<unsigned int>(std::cout, " "));
  std::cout << '\n';

  //auto[addr_size, n_macs, macs_ptr] = dp.get_tlv(1);
  auto[n_macs, addr_size, macs_ptr] = dp.get_tlv(1);

  for(int i=0; i < n_macs; i++)
  {
    std::string aux_mac;
    for(int j=0; j < addr_size; j++)
      aux_mac.push_back( macs_ptr[j + i*addr_size] );

    aux_macs.push_back( aux_mac );

  }
  
  printf("FOUND_MACS  : %i, %i : %02x:%02x:%02x:%02x:%02x:%02x \n", addr_size, n_macs,
         macs_ptr[0],macs_ptr[1],macs_ptr[2],macs_ptr[3],macs_ptr[4],macs_ptr[5] );
  printf("FOUND_MACS2 : %i, %i : %02x:%02x:%02x:%02x:%02x:%02x \n", addr_size, n_macs,
         macs_ptr[6],macs_ptr[7],macs_ptr[8],macs_ptr[9],macs_ptr[10],macs_ptr[11] );
 
  auto[ n_id, id_size, id_data_begin] = dp.get_tlv(2); 
 
  auto id_ptr = (const unsigned short *) id_data_begin; 
  printf("FOUND_IDS : %i, %i : %04x:%04x:%04x:%04x:%04x\n", 
         id_size, n_id, id_ptr[0], id_ptr[1], id_ptr[2], id_ptr[3], id_ptr[4] );
  
  //FILL mac addresses
  desc[HW_VID]    = id_ptr[0];
  desc[HW_PID]    = id_ptr[1];
  desc[HW_SS_VID] = id_ptr[3];
  desc[HW_SS_PID] = id_ptr[4];
  desc[SW_VID]    = id_ptr[0];
  desc[SW_PID]    = id_ptr[1];
  desc[SW_CLID]   = id_ptr[2];
  desc[SW_FID]    = id_ptr[3];
  desc[SW_VERID]  = id_ptr[4];

  auto dev_info = device_information(desc, intf_name, src_mac, aux_macs);
  //advancing ctrl and data offsets
  if(adv_tlv) dp.advance_tlv(2);

  return dev_info;

}

std::string
DatalinkPacket::device_information::serialize_macs() const
{
  std::string out;

  std::ranges::for_each(_macs, [&](auto src_mac){
    out += src_mac;

  }, &mac_params::src_mac);

  return out;
}

void DatalinkPacket::device_information::set_timeout( ulong timeout )
{
  _timeout = timeout;
}


bool DatalinkPacket::device_information::assess_expiration()
{
  bool any_port_expired = false;
  auto mark = std::chrono::system_clock::now();

  std::ranges::for_each(_macs, [&](auto mac)
  {
    auto elapsed_time = (mark - mac.get_keepalive() );
    auto seconds_elapsed = std::chrono::duration_cast<std::chrono::seconds>(elapsed_time).count();
    //only true if time elapsed and the link was up
    if( (seconds_elapsed > _timeout) && mac.link_status )
    {
      mac.expired();
      any_port_expired |= true;
    }

  });

  //if all ports are dead the device is down
  _still_alive = std::ranges::any_of(_macs, std::mem_fn(&mac_params::link_status) );

  return any_port_expired; 
 
}


std::vector<std::string>
DatalinkPacket::device_information::get_recently_exp_macs()
{
  std::vector<std::string> out;
  std::ranges::for_each(_macs, [&](auto mac)
  {
    if ( !mac.link_status && mac.last_link_status )
      out.push_back( mac.src_mac );

    mac.last_link_status = mac.link_status;   
    
  } );

  return out;
}

bool DatalinkPacket::device_information::mac_exists( std::string mac) const
{
  bool exists = std::ranges::any_of(_macs, equal_to{mac}, &mac_params::src_mac);
  printf("DL_SM ME: %02x:%02x:%02x:%02x:%02x:%02x\n", 
         mac[0], mac[1], mac[2],
         mac[3], mac[4], mac[5] );

  for(auto cmac : _macs )
    printf("DL_SM ME2: %02x:%02x:%02x:%02x:%02x:%02x -> %i\n", 
           cmac.src_mac[0], cmac.src_mac[1], cmac.src_mac[2],
           cmac.src_mac[3], cmac.src_mac[4], cmac.src_mac[5], exists );

  return exists;
}

void DatalinkPacket::device_information::update_link_status(int intf_id, std::string src_mac, bool link_status)
{
  auto found_mac = std::ranges::find_if(_macs, equal_to{src_mac}, &mac_params::src_mac);

  found_mac->intf_name = std::to_string( intf_id );
  found_mac->set_keepalive();

  if( link_status ) found_mac->renew();
  else found_mac->expired();
} 

DatalinkPacket::device_information&
DatalinkPacket::device_information::operator=(const DatalinkPacket::device_information& rhs )
{
  _still_alive = rhs.is_device_accessible();
  _timeout     = rhs.get_timeout();
  _macs        = rhs.get_macs();
  
  return *this; 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////.............Shared memory methods..........///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//device exist if there is one 
bool DLinkSM::device_exists( const DatalinkPacket::device_information& cand_dev, bool& accessible) const
{
  bool ret = false;
  accessible = false;

  std::lock_guard lk(_dev_mu);

  printf("DL_SM DEV size : %i \n", _devices.size() );
  std::ranges::for_each(_devices, [&](auto dev)
  {
    auto match = std::ranges::all_of(dev.get_macs(),
    [&](auto mcp)
    {
      printf("DL_SM DE: %02x:%02x:%02x:%02x:%02x:%02x\n", 
             mcp.src_mac[0], mcp.src_mac[1], mcp.src_mac[2],
             mcp.src_mac[3], mcp.src_mac[4], mcp.src_mac[5] );
      return cand_dev.mac_exists(mcp.src_mac);
    });
    
    //must have matching mac lists
    if( match )
    {
      printf("DL_SM DE_FOUND\n");
      ret = true;
      accessible = dev.is_device_accessible();
    } 
  } );
  return ret;
}

DatalinkPacket::device_information& 
DLinkSM::get_device_info( const DatalinkPacket::device_information& dev_info )
{
  std::lock_guard lk(_dev_mu);

  auto found_dev = std::ranges::find_if( _devices, 
  [&](auto dev_macs )
  {
    return std::ranges::all_of(dev_macs,
    [&](auto mcp)
    {
      return dev_info.mac_exists(mcp.src_mac);
    });

  }, &DatalinkPacket::device_information::get_macs );

  if (found_dev == std::end(_devices) )
    throw std::out_of_range("DL: Device does not exists in device list.");

  return *found_dev;
}

DatalinkPacket::device_information& 
DLinkSM::get_device_by_src_mac( std::string mac_key )
{
  std::lock_guard lk(_dev_mu);
  printf("DL_SM : %02x:%02x:%02x:%02x:%02x:%02x\n", 
         mac_key[0], mac_key[1], mac_key[2],
         mac_key[3], mac_key[4], mac_key[5] );


  printf("DL_SM devices sizes : %i \n", _devices.size() );

  auto found_dev = 
  std::ranges::find_if(_devices, [&](auto mcps)
  {
    printf("DL_SM mcps_size :%i \n", mcps.size() );
    for(auto& mcp : mcps)
      printf("DL_SM CAND: %02x:%02x:%02x:%02x:%02x:%02x\n", 
             mcp.src_mac[0], mcp.src_mac[1], mcp.src_mac[2],
             mcp.src_mac[3], mcp.src_mac[4], mcp.src_mac[5] );

    return std::ranges::any_of(mcps, equal_to{ mac_key }, 
              &DatalinkPacket::device_information::mac_params::src_mac);
   
  }, &DatalinkPacket::device_information::get_macs );

  if (found_dev == std::end(_devices) )
    throw std::out_of_range("DL: Device does not exists in device list.");

  return *found_dev;
}

DatalinkPacket::device_information&
DLinkSM::update_device_status(int intf_id, std::string mac_key, 
                                   std::optional<bool> link_status,
                                   std::optional<accel_descriptor> desc )
{
  auto& dev = get_device_by_src_mac( mac_key );

  if( link_status ) dev.update_link_status(intf_id, mac_key, link_status.value() );
  if( desc ) dev.set_desc( desc.value() );
  
  return dev; 
}


