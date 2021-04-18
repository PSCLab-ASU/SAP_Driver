#include "include/layers/dlink_layer.h"
#include "include/layers/phy_layer.h"


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
  this->template register_cmd<FromPhy>(DatalinkPacket::set_mac, &class_type::_set_mac_addr);
  std::cout << "1)Complete DatalinkLayer functions" << std::endl;
  ////////////////////////////////////////////////////////////////////////////

}

template<typename InputType>
int DatalinkLayer<InputType>::_noop(DatalinkPacket&& in, DatalinkPktVec& out )
{
  std::cout << "Calling Datalink noop..." << std::endl;
  return 0;
}
  
template<typename InputType>
int DatalinkLayer<InputType>::_self_ds(DatalinkPacket&& in, DatalinkPktVec& out )
{
  std::cout << "Calling Datalink self_ds..." << std::endl;
  if( !is_inited() ) _init( out );
  if( (get_hb_count() % _lldp_interval) == 0 ) _self_ds_lldp_pkt_gen( out );

  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_self_us(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink self_us..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_cleanup_ds(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink cleanup_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_cleanup_us(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling Datalink cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int DatalinkLayer<InputType>::_set_mac_addr(DatalinkPacket&& in, DatalinkPktVec& out )
{

  std::cout << "Calling set_mac_addr..." << std::endl;
  return 0;

}

template<typename InputType>
void DatalinkLayer<InputType>::_self_ds_lldp_pkt_gen( DatalinkPktVec& out )
{
  PhyPacket phy_packet( PhyPacket::send );

  std::vector<uchar> phy_data = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  phy_packet.append_data( phy_data );
  
  out.push_back( phy_packet.get_base() );
}

template<typename InputType>
void DatalinkLayer<InputType>::_init( DatalinkPktVec& out )
{
   out.push_back( _req_mac_address() );
}

template<typename InputType>
DatalinkPacket DatalinkLayer<InputType>::_req_mac_address( )
{
  return PhyPacket( PhyPacket::get_mac ).get_base();
}
