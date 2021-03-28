#include "include/layers/phy_layer.h"

PhyPacket::PhyPacket( typename BasePacket::type base )
: BasePacket ( base )
{


}

template<typename InputType>
PhyLayer<InputType>::PhyLayer()
{
  using namespace common_layer_cmds;
  using class_type = PhyLayer<InputType>;

  const auto FromApp = Pipelineflow::FromApp;
  const auto FromPhy = Pipelineflow::FromPhy; 

  std::cout << "0)Registrating PhyLayer functions" << std::endl;
  this->template register_cmd<FromApp>(noop, &class_type::_noop);
  this->template register_cmd<FromPhy>(noop, &class_type::_noop);
  std::cout << "1)Registrating PhyLayer functions" << std::endl;
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  std::cout << "2)Registrating PhyLayer functions" << std::endl;
  this->template register_cmd<FromApp>(command1, &class_type::_func0);
  std::cout << "3)Registrating PhyLayer functions" << std::endl;
  this->template register_cmd<FromPhy>(command2, &class_type::_func1);
  std::cout << "4)Registrating PhyLayer functions" << std::endl;

}

template<typename InputType>
int PhyLayer<InputType>::_noop(PhyPacket&& in, PhyPktVec& out )
{
  std::cout << "Calling Phy func0..." << std::endl;
  return 0;
}
  
template<typename InputType>
int PhyLayer<InputType>::_self_ds(PhyPacket&& in, PhyPktVec& out )
{
  std::cout << "Calling Phy func1..." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_self_us(PhyPacket&& in, PhyPktVec& out )
{

  std::cout << "Calling Phy func2..." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_func0(PhyPacket&& in, PhyPktVec& out )
{

  std::cout << "Calling Phy func3..." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_func1(PhyPacket&& in, PhyPktVec& out )
{

  std::cout << "Calling Phy func4..." << std::endl;
  return 0;
}
