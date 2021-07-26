#include "include/layers/net_layer.h"
#include "include/layers/packets/trans_packet.h"
#include <sstream>


template<typename InputType>
NetworkLayer<InputType>::NetworkLayer()
{
  
  using namespace common_layer_cmds;
  using class_type = NetworkLayer<InputType>;

  const auto FromApp = Pipelineflow::FromApp;
  const auto FromPhy = Pipelineflow::FromPhy; 

  std::cout << "0)Registrating NetworkLayer functions" << std::endl;
  this->template register_cmd<FromApp>(noop, &class_type::_noop);
  this->template register_cmd<FromPhy>(noop, &class_type::_noop);
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  this->template register_cmd<FromApp>(cleanup, &class_type::_cleanup_ds);
  this->template register_cmd<FromPhy>(cleanup, &class_type::_cleanup_us);
  this->template register_cmd<FromPhy>(discovery, &class_type::_track_device);
  this->template register_cmd<FromPhy>(NetworkPacket::keep_alive, &class_type::_keep_alive);
  this->template register_cmd<FromPhy>(NetworkPacket::deactivate_port, &class_type::_deactivate_port);

}

template<typename InputType>
int NetworkLayer<InputType>::_noop(NetworkPacket&& in, NetworkPktVec& out )
{
  std::cout << "Calling Network noop..." << std::endl;
  return 0;
}
  
template<typename InputType>
int NetworkLayer<InputType>::_self_ds(NetworkPacket&& in, NetworkPktVec& out )
{
  std::cout << "Calling Network self_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int NetworkLayer<InputType>::_self_us(NetworkPacket&& in, NetworkPktVec& out )
{

  std::cout << "Calling Network self_us..." << std::endl;
  return 0;
}

template<typename InputType>
int NetworkLayer<InputType>::_cleanup_ds(NetworkPacket&& in, NetworkPktVec& out )
{

  std::cout << "Calling Network cleanup_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int NetworkLayer<InputType>::_cleanup_us(NetworkPacket&& in, NetworkPktVec& out )
{

  std::cout << "Calling Network cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int NetworkLayer<InputType>::_track_device(NetworkPacket&& in, NetworkPktVec& out )
{
  printf("NET _track_device...\n");

  auto dev_info = NetworkPacket::device_information::deserialize( in );
  dev_info.set_id( _sm.get_num_devices() + 1 );

  //must match all the mac addresses to be an exact match
  bool exists = _sm.device_exists( dev_info );
  printf("NET device_exists : exists = %i \n", exists); 

  if( !exists ){
    _sm.add_device_information( dev_info );
  }

  //artifical noop
  out.push_back(NetworkPacket{} );
  return 0;

}

template<typename InputType>
int NetworkLayer<InputType>::_deactivate_port(NetworkPacket&& in, NetworkPktVec& out )
{
  unsigned char dev_idx = 0;
  auto[mac_sz,  n_macs,  mac_ptr ] = in.get_tlv(0); //src_mac
  auto mac = std::string((char *) mac_ptr, mac_sz);

  if( _sm.device_exists( mac ) )
  {
    auto& dev = _sm.get_device_info( mac, dev_idx );
    dev.deactivate_port( mac );

    if(dev.is_inaccessible() ) 
    {
      auto dev_id = dev.get_id();
      NetworkPacket np( TransportPacket::deactivate_device );
      np.append_ctrl_data( (unsigned char) dev_id.size() );
      np.append_ctrl_data((unsigned char)  1 );
      np.append_data( 1 , (const unsigned char *) dev_id.c_str() );
      out.push_back( np );
      printf("NET LOST Device %s \n", dev_id.c_str() );
    }

  }else printf("NET Mac doesn't exists\n");

  return 0;
}
template<typename InputType>
int NetworkLayer<InputType>::_keep_alive(NetworkPacket&& in, NetworkPktVec& out )
{
  printf("NET KEEP_ALIVE\n");
  accel_descriptor acd;
  auto[mac_sz,  n_macs,  mac_ptr ] = in.get_tlv(5); //src_mac
  auto[desc_sz, n_descs, desc_ptr] = in.get_tlv(0); //descs
  auto[cong_sz, n_congs, cong_ptr] = in.get_tlv(1); //link congest
  printf("NET MAC: %i, %i, \n", mac_sz, n_macs);
  printf("NET DESC: %i, %i, \n", desc_sz, n_descs);
  printf("NET LCONG: %i, %i, \n", cong_sz, n_congs);
 
  printf("NET MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
         mac_ptr[0], mac_ptr[1], mac_ptr[2],mac_ptr[3], mac_ptr[4], mac_ptr[5]); 

  printf("NET DATA : %llu : %td : %td : ", 
        (in.get_data<false>().size() ),
        (in.get_data() - in.get_data<false>().data()),
        (ptrdiff_t) (mac_ptr - in.get_data<false>().data() ) );
  std::ranges::for_each( in.get_data<false>(), [](auto v)
  {
    printf( "%02x,", v );
  });

 
  std::string mac = std::string( (char *) mac_ptr, 6);
  for(int i=HW_VID; i < ACCEL_ID_END; i++) 
  {
    unsigned int desc = ((ushort *) desc_ptr)[i];
    printf("NET DESC[%i] = %04x\n", i, desc);

    acd[(accel_desc_param)i] = desc;
  }

  ushort link_cong = ( (ushort *) cong_ptr)[0];
  printf("NET LCONG = %04x\n", link_cong);

  unsigned char dev_idx = 0;
  auto& dev = _sm.get_device_info( mac, dev_idx );
  auto avg_lcong = dev.get_avg_congest();

  //////////////////////////////////////////////////////
  auto dev_id = dev.get_id();
  dev.set_lcong( mac, link_cong ); 
  dev.set_desc( acd );
  //////////////////////////////////////////////////////
 
  in.set_op( TransportPacket::keep_alive ); 
  //returns index of the device
  in.append_ctrl_data( (unsigned char) dev_id.size() );
  in.append_ctrl_data((unsigned char)  1 );
  in.append_data( 1 , (const unsigned char *) dev_id.c_str() );
  ///////////////////////////////////////////////////////
  in.append_ctrl_data( (unsigned char)  sizeof(avg_lcong) );
  in.append_ctrl_data( (unsigned char)  1       );
  in.append_data( 1 ,  (const unsigned char *) &avg_lcong );
  ///////////////////////////////////////////////////////
  out.push_back( std::forward<decltype(in)>(in) );
  return 0; 
  
}

