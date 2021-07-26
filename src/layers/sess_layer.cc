#include "include/layers/sess_layer.h"

template<typename InputType>
SessionLayer<InputType>::SessionLayer()
{
  using namespace common_layer_cmds;
  //using namespace SessionPacket;

  using class_type = SessionLayer<InputType>;

  const auto FromApp = Pipelineflow::FromApp;
  const auto FromPhy = Pipelineflow::FromPhy; 

  std::cout << "0)Registrating SessionLayer functions" << std::endl;
  this->template register_cmd<FromApp>(noop, &class_type::_noop);
  this->template register_cmd<FromPhy>(noop, &class_type::_noop);
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  this->template register_cmd<FromApp>(cleanup, &class_type::_cleanup_ds);
  this->template register_cmd<FromPhy>(cleanup, &class_type::_cleanup_us);
  this->template register_cmd<FromPhy>(SessionPacket::device_info, &class_type::_device_info);
  this->template register_cmd<FromPhy>(SessionPacket::deactivate_device, &class_type::_deactivate_device);

}

template<typename InputType>
int SessionLayer<InputType>::_noop(SessionPacket&& in, SessionPktVec& out )
{
  std::cout << "Calling Session noop..." << std::endl;
  return 0;
}
  
template<typename InputType>
int SessionLayer<InputType>::_self_ds(SessionPacket&& in, SessionPktVec& out )
{
  std::cout << "Calling Session self_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_self_us(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session self_us..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_cleanup_ds(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session cleanup_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_cleanup_us(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_deactivate_device(SessionPacket&& in, SessionPktVec& out )
{
  std::cout << "Calling _deatvieate_device..." << std::endl;
  auto[dev_id_sz,  n_dev_ids,  dev_id_ptr]  = in.get_tlv(0); //device id
  uchar dev_id = dev_id_ptr[0];
  ushort id = ( ushort) dev_id;

  auto& dev = _sm.get_device_info( std::to_string(id) );
  dev.deactivate();
    
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_device_info(SessionPacket&& in, SessionPktVec& out )
{

  auto device = SessionPacket::device_information::deserialize( in );
  
  if ( _sm.device_exists( device.get_id() ) ) 
  {
    printf("SESS Updating existing device: %s\n", device.get_id().c_str() );
    auto& dev = _sm.get_device_info( device.get_id() );
    dev = device;
  }
  else 
  {
    printf("SESS Registering new device : %s\n", device.get_id().c_str() ); 
    _sm.add_device_information( device );
  }

  return 0;
}
