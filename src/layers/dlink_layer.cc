#include "include/layers/dlink_layer.h"

DatalinkPacket::DatalinkPacket( typename BasePacket::type base )
: BasePacket ( base )
{


}

template<typename InputType>
DatalinkLayer<InputType>::DatalinkLayer()
{
  using namespace common_layer_cmds;
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
  std::cout << "1)Complete DatalinkLayer functions" << std::endl;
  ////////////////////////////////////////////////////////////////////////////

}

template<typename InputType>
int DatalinkLayer<InputType>::_noop(DatalinkPacket&& in, DatalinkPktVec& out )
{
  std::cout << "Calling Datalink func0..." << std::endl;
  return 0;
}
  
template<typename InputType>
int DatalinkLayer<InputType>::_self_ds(DatalinkPacket&& in, DatalinkPktVec& out )
{
  std::cout << "Calling Datalink func1..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_self_us(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink func2..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_cleanup_ds(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink func3..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_cleanup_us(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink func4..." << std::endl;
  return 0;
}
