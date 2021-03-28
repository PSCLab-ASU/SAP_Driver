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
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  this->template register_cmd<FromApp>(cleanup, &class_type::_cleanup_ds);
  this->template register_cmd<FromPhy>(cleanup, &class_type::_cleanup_us);

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
int PhyLayer<InputType>::_cleanup_ds(PhyPacket&& in, PhyPktVec& out )
{

  std::cout << "Calling Phy func3..." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_cleanup_us(PhyPacket&& in, PhyPktVec& out )
{

  std::cout << "Calling Phy func4..." << std::endl;
  return 0;
}
