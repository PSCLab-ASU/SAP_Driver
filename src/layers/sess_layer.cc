#include "include/layers/sess_layer.h"

SessionPacket::SessionPacket( typename BasePacket::type base )
: BasePacket ( base )
{


}

template<typename InputType>
SessionLayer<InputType>::SessionLayer()
{
  using namespace common_layer_cmds;
  using class_type = SessionLayer<InputType>;

  const auto FromApp = Pipelineflow::FromApp;
  const auto FromPhy = Pipelineflow::FromPhy; 

  std::cout << "0)Registrating SessionLayer functions" << std::endl;
  this->template register_cmd<FromApp>(noop, &class_type::_noop);
  this->template register_cmd<FromPhy>(noop, &class_type::_noop);
  std::cout << "1)Registrating SessionLayer functions" << std::endl;
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  std::cout << "2)Registrating SessionLayer functions" << std::endl;
  this->template register_cmd<FromApp>(command1, &class_type::_func0);
  std::cout << "3)Registrating SessionLayer functions" << std::endl;
  this->template register_cmd<FromPhy>(command2, &class_type::_func1);
  std::cout << "4)Registrating SessionLayer functions" << std::endl;

}

template<typename InputType>
int SessionLayer<InputType>::_noop(SessionPacket&& in, SessionPktVec& out )
{
  std::cout << "Calling Session func0..." << std::endl;
  return 0;
}
  
template<typename InputType>
int SessionLayer<InputType>::_self_ds(SessionPacket&& in, SessionPktVec& out )
{
  std::cout << "Calling Session func1..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_self_us(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session func2..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_func0(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session func3..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_func1(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session func4..." << std::endl;
  return 0;
}
