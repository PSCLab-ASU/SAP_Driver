#include "include/layers/net_layer.h"

NetworkPacket::NetworkPacket( typename BasePacket::type base )
: BasePacket ( base )
{


}

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
  std::cout << "1)Registrating NetworkLayer functions" << std::endl;
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  std::cout << "2)Registrating NetworkLayer functions" << std::endl;
  this->template register_cmd<FromApp>(command1, &class_type::_func0);
  std::cout << "3)Registrating NetworkLayer functions" << std::endl;
  this->template register_cmd<FromPhy>(command2, &class_type::_func1);
  std::cout << "4)Registrating NetworkLayer functions" << std::endl;

}

template<typename InputType>
int NetworkLayer<InputType>::_noop(NetworkPacket&& in, NetworkPktVec& out )
{
  std::cout << "Calling Network func0..." << std::endl;
  return 0;
}
  
template<typename InputType>
int NetworkLayer<InputType>::_self_ds(NetworkPacket&& in, NetworkPktVec& out )
{
  std::cout << "Calling Network func1..." << std::endl;
  return 0;
}

template<typename InputType>
int NetworkLayer<InputType>::_self_us(NetworkPacket&& in, NetworkPktVec& out )
{

  std::cout << "Calling Network func2..." << std::endl;
  return 0;
}

template<typename InputType>
int NetworkLayer<InputType>::_func0(NetworkPacket&& in, NetworkPktVec& out )
{

  std::cout << "Calling Network func3..." << std::endl;
  return 0;
}

template<typename InputType>
int NetworkLayer<InputType>::_func1(NetworkPacket&& in, NetworkPktVec& out )
{

  std::cout << "Calling Network func4..." << std::endl;
  return 0;
}
