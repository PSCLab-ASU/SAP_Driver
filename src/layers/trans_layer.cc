#include "include/layers/trans_layer.h"

TransportPacket::TransportPacket( typename BasePacket::type base )
: BasePacket ( base )
{


}

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

}

template<typename InputType>
int TransportLayer<InputType>::_noop(TransportPacket&& in, TransportPktVec& out )
{
  std::cout << "Calling Transport func0..." << std::endl;
  return 0;
}
  
template<typename InputType>
int TransportLayer<InputType>::_self_ds(TransportPacket&& in, TransportPktVec& out )
{
  std::cout << "Calling Transport func1..." << std::endl;
  return 0;
}

template<typename InputType>
int TransportLayer<InputType>::_self_us(TransportPacket&& in, TransportPktVec& out )
{

  std::cout << "Calling Transport func2..." << std::endl;
  return 0;
}

template<typename InputType>
int TransportLayer<InputType>::_cleanup_ds(TransportPacket&& in, TransportPktVec& out )
{

  std::cout << "Calling Transport func3..." << std::endl;
  return 0;
}

template<typename InputType>
int TransportLayer<InputType>::_cleanup_us(TransportPacket&& in, TransportPktVec& out )
{

  std::cout << "Calling Transport func4..." << std::endl;
  return 0;
}
