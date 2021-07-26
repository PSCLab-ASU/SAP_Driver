#include "include/layers/dlink_layer.h"
#include "include/layers/phy_layer.h"
#include "include/layers/packets/dlink_packet.h"
#include "include/layers/packets/net_packet.h"

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
  std::cout << "Calling dlink track_device..." << std::endl;
  std::string src_mac;
  bool dev_accessible =false;
 
  //will already be timestamped! 
  auto dev_info = DatalinkPacket::device_information::deserialize( in, src_mac );
  
  if( !_sm.device_exists( dev_info, dev_accessible ) ){
    dev_info.set_timeout(_device_timeout);
    _sm.add_device_information( std::move(dev_info) );
    //use the last device on the list
    //send desc, src_mac, timout val
    _upstream_dev_info( in, out );
  }
  else{
    auto dev_update = _sm.update_device_status(
                          in.get_intf_id(), 
                          src_mac, true, 
                          dev_info.get_desc() );

    //update desc & use src_mac as a key
    //new_timeout
    auto o_dev_upt = std::optional( dev_update );
    _upstream_dev_info(in, out, o_dev_upt );
  }

  //send keep alive
  _upstream_dev_info(in, out, std::optional(src_mac) );
   
  //TBD: increment payload offsets and pass it up
  //out.push_back( DatalinkPacket{} );

  return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////........internal........////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

template<typename InputType> template<typename DevUpdate>
void DatalinkLayer<InputType>::_upstream_dev_info( DatalinkPacket& prev, DatalinkPktVec& out, std::optional<DevUpdate> o_dev_update )
{
  const unsigned char mac_size  = 0x06;
  unsigned char n_macs          = 0x01;
  DatalinkPacket dp;

  if constexpr( std::is_same_v<DevUpdate, std::string> )
  {
    printf("DL3 KEEPALIVE : %i\n", NetworkPacket::keep_alive);
    prev.set_op( NetworkPacket::keep_alive );
    //TLV LAST : MACS //
    prev.insert_ctrl_data(1, 0); //<--filler for cnt
    /////////////////Eth Padding/////////////////////
    prev.append_ctrl_data( (unsigned char) 1 );
    prev.append_ctrl_data( (unsigned char) 6 );
    /////////////////////////////////////////////////
    prev.append_ctrl_data( (unsigned char) 1 );
    prev.append_ctrl_data( (unsigned char) mac_size);
    prev.append_data( o_dev_update->size(), (const unsigned char *) o_dev_update->c_str() );
    //this function will merge the extra data from the previous layer
    //this data gets the efficiency numbers
    printf("NET DLINK mac: %02x:%02x:%02x:%02x:%02x:%02x \n", 
           o_dev_update->c_str()[0],
           o_dev_update->c_str()[1],
           o_dev_update->c_str()[2],
           o_dev_update->c_str()[3],
           o_dev_update->c_str()[4],
           o_dev_update->c_str()[5]
          );
    out.push_back(prev);
  }
  else 
  {
    printf("DL3 DISCOVERY\n");
    DatalinkPacket::device_information * dev_ptr = nullptr; 
    dp.set_op( common_layer_cmds::discovery );

    if constexpr( std::is_same_v<DevUpdate, NullType> ){
      dev_ptr = &_sm.get_last_dev_info();
    }
    else{
      dev_ptr = &o_dev_update.value();     
    }
    //get the last device on the lisr
    //auto serial_desc = dev_ptr->serialize_desc();
    auto serial_macs = dev_ptr->serialize_macs();
    /////////////////////////////////////////////
    //dp.append_ctrl_data( serial_desc.size() );
    //dp.append_ctrl_data( 1 );
    dp.append_ctrl_data( (unsigned char) mac_size);
    dp.append_ctrl_data( (unsigned char) dev_ptr->get_nmacs() );
    //dp.append_data( serial_desc.size(), (const unsigned char *) serial_desc.data() );
    printf("DL3 DISCOVERY mac_size : %i\n", serial_macs.size() );
    dp.append_data( serial_macs.size(), (const unsigned char *) serial_macs.c_str() );
    //this function will merge the extra data from the previous layer
    //this data gets the efficiency numbers
    //dp.fuse_extra( prev.get_base() );
    auto s = dp.get_data<false>();
    printf(" DL3 DATA : ");
    for(int i=0; i < serial_macs.size(); i++)
    {
      printf("%02x,", serial_macs[i] );
    } 
    out.push_back(dp);
    
  }
}

template<typename InputType>
void DatalinkLayer<InputType>::_check_invalidate( DatalinkPktVec& out )
{
  auto device_update = std::mem_fn(&DatalinkPacket::device_information::assess_expiration);

  auto flow = _sm._devices | std::views::filter( device_update );

  std::lock_guard lk( _sm._dev_mu );

  for(auto& cand_dev : flow ) {
    auto exp_macs = cand_dev.get_recently_exp_macs();
    for( auto mac : exp_macs )
    {
      printf("DLINK  Deactivating %02x:%02x:%02x:%02x:%02x:%02x\n",
             mac[0], mac[1],mac[2],mac[3],mac[4],mac[5] );

      DatalinkPacket dp( NetworkPacket::deactivate_port );
      dp.append_ctrl_data( (unsigned char) 6);
      dp.append_ctrl_data( (unsigned char) 1);
      dp.append_data( mac.size(), (const unsigned char *) mac.c_str() );

      out.push_back(dp);
    }
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


