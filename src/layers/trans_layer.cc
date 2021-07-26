#include "include/layers/trans_layer.h"
#include "include/layers/packets/sess_packet.h"

template<typename InputType>
TransportLayer<InputType>::TransportLayer()
{
  using namespace common_layer_cmds;
  using class_type = TransportLayer<InputType>;

  const auto FromApp = Pipelineflow::FromApp;
  const auto FromPhy = Pipelineflow::FromPhy; 

  std::cout << "0)Registrating TransportLayer functions" << std::endl;
  this->template register_cmd<FromApp>(noop, &class_type::_noop);
  this->template register_cmd<FromPhy>(noop, &class_type::_noop);
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  this->template register_cmd<FromApp>(cleanup, &class_type::_cleanup_ds);
  this->template register_cmd<FromPhy>(cleanup, &class_type::_cleanup_us);
  this->template register_cmd<FromPhy>(TransportPacket::keep_alive, 
                                       &class_type::template pass_through<SessionPacket::device_info> );
  this->template register_cmd<FromPhy>(TransportPacket::deactivate_device, 
                                       &class_type::template pass_through<SessionPacket::deactivate_device> );

}

template<typename InputType>
int TransportLayer<InputType>::_noop(TransportPacket&& in, TransportPktVec& out )
{
  std::cout << "Calling Transport noop..." << std::endl;
  return 0;
}
  
template<typename InputType>
int TransportLayer<InputType>::_self_ds(TransportPacket&& in, TransportPktVec& out )
{
  std::cout << "Calling Transport self_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int TransportLayer<InputType>::_self_us(TransportPacket&& in, TransportPktVec& out )
{

  std::cout << "Calling Transport self_us..." << std::endl;
  return 0;
}

template<typename InputType>
int TransportLayer<InputType>::_cleanup_ds(TransportPacket&& in, TransportPktVec& out )
{

  std::cout << "Calling Transport cleanup_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int TransportLayer<InputType>::_cleanup_us(TransportPacket&& in, TransportPktVec& out )
{

  std::cout << "Calling Transport cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int TransportLayer<InputType>::_track_device(TransportPacket&& in, TransportPktVec& out )
{

  std::cout << "Calling Transport _track_device..." << std::endl;
  return 0;
}
