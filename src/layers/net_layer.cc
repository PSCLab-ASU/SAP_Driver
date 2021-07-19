#include "include/layers/net_layer.h"

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
  bool exists = _sm.exact_match( dev_info );

  out.push_back( NetworkPacket{} );
  return 0;

  if( exists )
  {
    auto& dev = _sm.get_device_info( dev_info );

    //update everything except for the state of aux links
    dev.update( dev_info ); 

    return 0; 
  }
  else _sm.add_device_information( dev_info );

  auto np = _packetize_discovery( dev_info ); 

  out.push_back( np );

  return 0;
}

template<typename InputType>
int NetworkLayer<InputType>::_keep_alive(NetworkPacket&& in, NetworkPktVec& out )
{
  printf("NET KEEP_ALIVE\n");
  out.push_back( NetworkPacket{} );
  return 0; 
}

template<typename InputType>
NetworkPacket NetworkLayer<InputType>::_packetize_discovery( const NetworkPacket::device_information& device  )
{
  const size_t four = 4;
  NetworkPacket np( common_layer_cmds::discovery );
 
  const unsigned char is_active = (const uchar) device.is_active();
  auto desc = device.serialize_desc();
  size_t desc_size = desc.size();
  std::string id  = device.get_id();
  size_t id_size  = id.size();

  ////////////////////////////////////////////////////////////
  /////////////////....adding active....//////////////////////
  np.append_ctrl_data(1, &is_active);
  /////////////////....adding id....//////////////////////////
  np.append_ctrl_data(4, (const unsigned char *) &id_size );
  np.append_data(id_size,(const unsigned char *) id.c_str() );
  ////////////////////////////////////////////////////////////
  /////////////////....adding desc....////////////////////////
  np.append_ctrl_data(4, (const unsigned char *) &desc_size );
  np.append_data(desc.size(), desc.data() );
  ////////////////////////////////////////////////////////////
  /////////////////....adding link congestion..../////////////
  size_t cong = device.get_average_congestion();
  np.append_ctrl_data(4, (const unsigned char *) &four );
  np.append_data(4, (const unsigned char *) &cong );
  ////////////////////////////////////////////////////////////
   
  ////////////////////////////////////////////////////////////
  ////////////////.....adding extra data...../////////////////
  auto [byte_size, ptr] =  device.get_extra();
  np.append_ctrl_data(sizeof(decltype(byte_size) ), 
                      (const unsigned char *) &byte_size);
  np.append_data(byte_size, ptr);
  ////////////////////////////////////////////////////////////

  return np;
}
