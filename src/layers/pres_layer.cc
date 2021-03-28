#include "include/layers/pres_layer.h"

PresentationPacket::PresentationPacket( typename BasePacket::type base )
: BasePacket ( base )
{


}

template<typename InputType>
PresentationLayer<InputType>::PresentationLayer()
{
  using namespace common_layer_cmds;
  using class_type = PresentationLayer<InputType>;

  const auto FromApp = Pipelineflow::FromApp;
  const auto FromPhy = Pipelineflow::FromPhy; 

  std::cout << "0)Registrating PresentationLayer functions" << std::endl;
  this->template register_cmd<FromApp>(noop, &class_type::_noop);
  this->template register_cmd<FromPhy>(noop, &class_type::_noop);
  std::cout << "1)Registrating PrensetationLayer functions" << std::endl;
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  std::cout << "2)Registrating PrensetationLayer functions" << std::endl;
  this->template register_cmd<FromApp>(command1, &class_type::_func0);
  std::cout << "3)Registrating PrensetationLayer functions" << std::endl;
  this->template register_cmd<FromPhy>(command2, &class_type::_func1);
  std::cout << "4)Registrating PrensetationLayer functions" << std::endl;

}

template<typename InputType>
int PresentationLayer<InputType>::_noop(PresentationPacket&& in, PresentationPktVec& out )
{
  std::cout << "Calling Presentation func0..." << std::endl;
  return 0;
}
  
template<typename InputType>
int PresentationLayer<InputType>::_self_ds(PresentationPacket&& in, PresentationPktVec& out )
{
  std::cout << "Calling Presentation func1..." << std::endl;
  return 0;
}

template<typename InputType>
int PresentationLayer<InputType>::_self_us(PresentationPacket&& in, PresentationPktVec& out )
{

  std::cout << "Calling Presentation func2..." << std::endl;
  return 0;
}

template<typename InputType>
int PresentationLayer<InputType>::_func0(PresentationPacket&& in, PresentationPktVec& out )
{

  std::cout << "Calling Presentation func3..." << std::endl;
  return 0;
}

template<typename InputType>
int PresentationLayer<InputType>::_func1(PresentationPacket&& in, PresentationPktVec& out )
{

  std::cout << "Calling Presentation func4..." << std::endl;
  return 0;
}
