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
  std::cout << "1)Registrating DatalinkLayer functions" << std::endl;
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  std::cout << "2)Registrating DatalinkLayer functions" << std::endl;
  this->template register_cmd<FromApp>(command1, &class_type::_func0);
  std::cout << "3)Registrating DatalinkLayer functions" << std::endl;
  this->template register_cmd<FromPhy>(command2, &class_type::_func1);
  std::cout << "4)Registrating DatalinkLayer functions" << std::endl;

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
int DatalinkLayer<InputType>::_func0(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink func3..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_func1(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink func4..." << std::endl;
  return 0;
}
