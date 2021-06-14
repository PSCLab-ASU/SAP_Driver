#include "include/layers/dlink_layer.h"
#include "include/layers/phy_layer.h"
#include "include/layers/packets/dlink_packet.h"

template<typename InputType>
DatalinkLayer<InputType>::DatalinkLayer()
{
  using namespace common_layer_cmds;
  //using namespace DatalinkPacket;

  using class_type = DatalinkLayer<InputType>;

  const auto FromApp = Pipelineflow::FromApp;
  const auto FromPhy = Pipelineflow::FromPhy; 

  std::cout << "0)Registrating DatalinkLayer functions" << std::endl;
  this->template register_cmd<FromApp>(noop, &class_type::_noop);
  this->template register_cmd<FromPhy>(noop, &class_type::_noop);
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  this->template register_cmd<FromApp>(cleanup, &class_type::_cleanup_ds);
  this->template register_cmd<FromPhy>(cleanup, &class_type::_cleanup_us);
  this->template register_cmd<FromPhy>(DatalinkPacket::set_mac, &class_type::_set_mac_addr);
  this->template register_cmd<FromPhy>(discovery, &class_type::_track_device);
  std::cout << "1)Complete DatalinkLayer functions" << std::endl;
  ////////////////////////////////////////////////////////////////////////////

}

template<typename InputType>
int DatalinkLayer<InputType>::_noop(DatalinkPacket&& in, DatalinkPktVec& out )
{
  std::cout << "Calling Datalink noop..." << std::endl;
  return 0;
}
  
template<typename InputType>
int DatalinkLayer<InputType>::_self_ds(DatalinkPacket&& in, DatalinkPktVec& out )
{
  std::cout << "Calling Datalink self_ds..." << std::endl;
  //if( (get_hb_count() % _lldp_interval) == 0 ) 

  //this function checks all the device timers and invalidate if necessary
  _check_invalidate( out );

  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_self_us(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink self_us..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_cleanup_ds(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink cleanup_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_cleanup_us(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_set_mac_addr(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling set_mac_addr..." << std::endl;
  return 0;

}

template<typename InputType>
int DatalinkLayer<InputType>::_track_device(DatalinkPacket&& in, DatalinkPktVec& out )
{
  std::cout << "Calling track_device..." << std::endl;

  auto raw_dev_info = in.try_extract_dev_info();

  if( raw_dev_info )
  {
    auto dev_info = DatalinkPacket::device_information::deserialize( raw_dev_info.value() );

    if( !_sm.device_exists( dev_info ) )
    {
      dev_info.stamp_time();
      dev_info.set_timeout(_device_timeout);

      _sm.add_device_information( std::move(dev_info) );

      //send it upstream
      auto res = _packetize_discovery( dev_info );
      out.push_back(res);
    }
    else
    {
      //only checks for mac address
      auto& reged_dev = _sm.get_device_info( dev_info );
      bool em         = _sm.exact_match( dev_info );
      bool was_active = reged_dev.is_active();

      reged_dev.stamp_time();
      reged_dev.activate();

      //updated device_information with latest
      //discovery parameters
      if( !em ) reged_dev = dev_info;
     
      auto res = _packetize_discovery( reged_dev );
      out.push_back(res);

    }
  }
  else{ std::cout << "No Device information in packet" << std::endl; }

  return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////........internal........////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

template<typename InputType>
void DatalinkLayer<InputType>::_check_invalidate( DatalinkPktVec& out )
{
  auto is_changed = std::mem_fn(&DatalinkPacket::device_information::has_state_changed);

  auto flow = _sm._devices.second | std::views::filter( is_changed );

  std::lock_guard lk( _sm._devices.first );

  std::ranges::for_each(_sm._devices.second, std::identity{}, 
                        &DatalinkPacket::device_information::update_still_alive);

  for(auto& cand_dev : flow ) {
    auto res = _packetize_discovery( cand_dev );
    out.push_back(res);
  }
                       
}

template<typename InputType>
void DatalinkLayer<InputType>::_init( DatalinkPktVec& out )
{
   out.push_back( _req_mac_address() );
}

template<typename InputType>
DatalinkPacket DatalinkLayer<InputType>::_req_mac_address( )
{
  return PhyPacket( PhyPacket::get_mac ).get_base();
}

template<typename InputType>
DatalinkPacket DatalinkLayer<InputType>::_packetize_discovery( const DatalinkPacket::device_information& in )
{
  DatalinkPacket dp( common_layer_cmds::discovery );
  const unsigned char len = (const uchar) in._macs.size();
  const unsigned char zeros[2] = {0x00, 0x00};
 
  //append the number of interfaces
  dp.append_ctrl_data(1, &len);
  auto desc = in.serialize_desc();

  std::ranges::for_each( in._macs, [&](auto link)
  {
    const unsigned char is_active = (const uchar) link.first;

    dp.append_ctrl_data(1, &is_active);
    dp.append_data(desc.size(), desc.data() );
    dp.append_data(2, zeros );
    dp.append_data(6, (const unsigned char *) link.second.c_str() );

  } );
   
  //add the upstream data
  auto [byte_size, ptr] =  in.get_extra();
  dp.append_ctrl_data(sizeof(decltype(byte_size) ), (unsigned char *) &byte_size);
  dp.append_data(byte_size, ptr);

  return dp;
}

