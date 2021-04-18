#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <initializer_list>
#include <iostream>
#include "include/layers/phy_layer.h"

PhyPacket::PhyPacket( typename BasePacket::type base )
: BasePacket ( base )
{


}

PhyPacket::PhyPacket( ushort op )
: BasePacket( std::make_shared<base_pipeline_data>() )
{
  set_op( op );
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
  this->template register_cmd<FromApp>(PhyPacket::send, &class_type::_send);
  this->template register_cmd<FromApp>(PhyPacket::get_mac, &class_type::_get_mac);

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
  std::cout << "Calling Phy self_ds..." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_self_us(PhyPacket&& in, PhyPktVec& out )
{

  std::cout << "read all socket(s) from here with recvFrom." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_cleanup_ds(PhyPacket&& in, PhyPktVec& out )
{

  std::cout << "Calling Phy cleanup_ds..." << std::endl;
  std::cout << "----Marking packet to opposite stream----" << std::endl;
  //sending to internal queue
  in.mark_as_resp();

  //push to output
  std::cout << "----forwarding packet to upstream----" << std::endl;
  out.push_back(in);
  
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_cleanup_us(PhyPacket&& in, PhyPktVec& out )
{

  std::cout << "Calling Phy cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_get_mac(PhyPacket&& in, PhyPktVec& out )
{
  std::cout << "Calling Phy cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_send(PhyPacket&& in, PhyPktVec& out )
{
  _internal_send( std::forward<PhyPacket>(in) );

  std::cout << "Calling Phy cleanup_us..." << std::endl;
  return 0;
}

template<typename InputType>
int PhyLayer<InputType>::_internal_send(PhyPacket&& in )
{
  int sockfd, n;
  int send_result = 0;
  struct sockaddr_ll socket_address; 
  return 0;
}
