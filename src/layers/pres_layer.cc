#include "include/layers/pres_layer.h"
#include "include/app_structs.h"
#include <cstdlib>

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
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  this->template register_cmd<FromApp>(cleanup, &class_type::_cleanup_ds);
  this->template register_cmd<FromPhy>(cleanup, &class_type::_cleanup_us);
  this->template register_cmd<FromApp>(PresentationPacket::get_devices,
                                       &class_type::template pass_through<SessionPacket::get_devices> );
  this->template register_cmd<FromPhy>(PresentationPacket::get_devices, &class_type::_get_devices );
}

template<typename InputType>
int PresentationLayer<InputType>::_noop(PresentationPacket&& in, PresentationPktVec& out )
{
  std::cout << "Calling Presentation noop..." << std::endl;
  return 0;
}
  
template<typename InputType>
int PresentationLayer<InputType>::_self_ds(PresentationPacket&& in, PresentationPktVec& out )
{
  std::cout << "Calling Presentation self_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int PresentationLayer<InputType>::_self_us(PresentationPacket&& in, PresentationPktVec& out )
{

  std::cout << "Calling Presentation self_us..." << std::endl;
  return 0;
}

template<typename InputType>
int PresentationLayer<InputType>::_cleanup_ds(PresentationPacket&& in, PresentationPktVec& out )
{

  std::cout << "Calling Presentation cleanup_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int PresentationLayer<InputType>::_cleanup_us(PresentationPacket&& in, PresentationPktVec& out )
{

  std::cout << "Calling Presentation cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int PresentationLayer<InputType>::_func0(PresentationPacket&& in, PresentationPktVec& out )
{

  std::cout << "Calling Presentation func0..." << std::endl;
  return 0;
}

template<typename InputType>
int PresentationLayer<InputType>::_get_devices(PresentationPacket&& in, PresentationPktVec& out )
{

  std::cout << "Calling Presentation _get_devices..." << std::endl;
  auto devs = app_intf::devices::deserialize( PipelineOutput(in.get_base() ) );
 
  out.push_back( devs.serialize().get_base() );

  return 0;
}
