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
  std::cout << "Calling networklayer _track_device..." << std::endl;
  return 0;
}

template<typename InputType>
NetworkPacket NetworkLayer<InputType>::_packetize_discovery( const NetworkPacket::device_information& device  )
{
  return NetworkPacket( common_layer_cmds::discovery );
}
