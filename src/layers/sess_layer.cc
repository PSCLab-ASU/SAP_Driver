#include "include/layers/sess_layer.h"

SessionPacket::SessionPacket( typename BasePacket::type base )
: BasePacket ( base )
{


}

template<typename InputType>
SessionLayer<InputType>::SessionLayer()
{
  using namespace common_layer_cmds;
  //using namespace SessionPacket;

  using class_type = SessionLayer<InputType>;

  const auto FromApp = Pipelineflow::FromApp;
  const auto FromPhy = Pipelineflow::FromPhy; 

  std::cout << "0)Registrating SessionLayer functions" << std::endl;
  this->template register_cmd<FromApp>(noop, &class_type::_noop);
  this->template register_cmd<FromPhy>(noop, &class_type::_noop);
  this->template register_cmd<FromApp>(self, &class_type::_self_ds);
  this->template register_cmd<FromPhy>(self, &class_type::_self_us);
  this->template register_cmd<FromApp>(cleanup, &class_type::_cleanup_ds);
  this->template register_cmd<FromPhy>(cleanup, &class_type::_cleanup_us);
  //this->template register_cmd<FromPhy>(discovery, &class_type::_track_device);

}

template<typename InputType>
int SessionLayer<InputType>::_noop(SessionPacket&& in, SessionPktVec& out )
{
  std::cout << "Calling Session noop..." << std::endl;
  return 0;
}
  
template<typename InputType>
int SessionLayer<InputType>::_self_ds(SessionPacket&& in, SessionPktVec& out )
{
  std::cout << "Calling Session self_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_self_us(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session self_us..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_cleanup_ds(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session cleanup_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int SessionLayer<InputType>::_cleanup_us(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling Session cleanup_us..." << std::endl;
  return 0;
}

/*template<typename InputType>
int SessionLayer<InputType>::_track_device(SessionPacket&& in, SessionPktVec& out )
{

  std::cout << "Calling _track_device..." << std::endl;
  return 0;
}*/
