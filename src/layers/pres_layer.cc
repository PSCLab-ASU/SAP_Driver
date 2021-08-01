#include "include/layers/packets/inout_packet.h"
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

  std::cout << "PRES Calling Presentation _get_devices..." << std::endl;
  
 /* auto d = in.get_data<false>();
  auto ctrl = in.get_ctrl<false>();

  auto ddo = in.get_data();
  auto delta = ddo - d.data();

  printf("PRES ctrl (size= %lu) = %p : ", ctrl.size(), ctrl.data() );
  for(auto byt : ctrl) printf("%02x:", byt);
  printf("\n");


  printf("PRES (size= %lu) = %p : %p : ", d.size(), d.data(), ddo );
  for(auto byt : d) printf("%02x:", byt);
  printf("\n");

  printf("PRES = " );
  for(int i = 0; i < d.size(); i++ ) printf("%02x:", ddo[i] );
  printf("\n");
*/
  auto devs = app_intf::devices::deserialize( PipelineOutput(in.get_base() ) );
  auto pkt  = devs.serialize();
  pkt.set_op( PresentationPacket::get_devices );
 
  out.push_back( pkt.get_base() );

  std::cout << "PRES Calling Presentation _get_devices completed..." << std::endl;

  return 0;
}
