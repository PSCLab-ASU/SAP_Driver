#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <ifaddrs.h>
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <netdb.h>
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
  this->template register_cmd<FromApp>(PhyPacket::set_intfs, &class_type::_set_intfs);
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
 
  sockaddr_ll from;
  socklen_t addrlen;

  unsigned char data[MAX_MTU];

  std::cout << "read all socket(s) from here with recvFrom." << std::endl;
  auto active_intfs = _sm._txrx_socks | std::views::filter([](auto& intf){ return (bool) intf; } );

  for( auto active_intf : active_intfs )
  {

    PhyPacket p( common_layer_cmds::noop );    
    p.set_intf_id( active_intf.get_id() );

    auto bytes = recvfrom(active_intf.get_rx_socket(), data, 
                          MAX_MTU, MSG_DONTWAIT | MSG_PEEK, (sockaddr *) &from, &addrlen); 

    if( bytes > 0 )
    {
      std::string src_mac =   { (char) from.sll_addr[0], (char) from.sll_addr[1],
                                (char) from.sll_addr[2], (char) from.sll_addr[3], 
                                (char) from.sll_addr[4], (char) from.sll_addr[5], (char) 0 };

      std::string type =  { (char) from.sll_addr[6], (char) from.sll_addr[7], (char) 0 };

      if( type == g_accelerator_type )
      {
        p.allocate_data( bytes );
        p.set_op( (ushort) data[0] );
        p.set_src( src_mac );
   
        auto new_bytes = recv(active_intf.get_rx_socket(), p.get_data().data(), MAX_MTU, MSG_DONTWAIT ); 

        out.push_back( p );
      }
      else{
        std::cout << "Not a SAP packet...." << std::endl;
      }
    }

  }


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
int PhyLayer<InputType>::_set_intfs(PhyPacket&& in, PhyPktVec& out )
{
  std::vector<std::string> intfs;
  size_t offset = 0;
  auto lens = in.get_ctrl_data();
  auto data_sz = in.get_data().size();

  std::string data = std::string( ( (char *) in.get_data().data() ) );

  if( (data_sz > 0) && ( lens.size() > 0 ) )
  {
    std::ranges::transform( lens, std::back_inserter(intfs), [&]( auto len )
    {
      std::string intf = data.substr(offset, len);
      offset += len;    
      return intf;
    } );

    //TBD COnfigure sockets
    bool can_configure = _configure_interfaces( intfs );

    if ( can_configure )
    {
      _activate_intfs(); 
    }
    else
    {
      std::cout << "**************************************" << std::endl;
      std::cout << "Could not configure any interfaces!!" << std::endl;
      std::cout << "**************************************" << std::endl;
    }


  }
  else
  {
    std::cout << "**************************************" << std::endl;
    std::cout << "PHY Set Intf: No Interfaces available!" << std::endl;
    std::cout << "**************************************" << std::endl;
  }
  
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
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

template<typename InputType>
bool PhyLayer<InputType>::_configure_interfaces( std::vector<std::string> intfs )
{
  ifaddrs * pifap = nullptr;
  auto initial_ptr = pifap;

  int res = getifaddrs( &pifap );

  while( pifap != NULL )
  {
    std::cout << "Interface Name : " << pifap->ifa_name << std::endl;

    //find intferace
    bool match = std::ranges::any_of(intfs, [&]( auto intf){
      return intf == std::string( pifap->ifa_name );
    }); 
  
    if( match)
    {
      std::string src;
      struct sockaddr_ll *s = (struct sockaddr_ll*)pifap->ifa_addr;

      //pull mac address
      for(int i = 0; i < 6; i++) src.push_back( s->sll_addr[i] );

      _sm.add_sock( ((sockaddr_ll *)pifap->ifa_addr)->sll_ifindex, src); 
    }

    pifap = pifap->ifa_next;
  }

  //at this point socket handles wil lbe created
  _activate_intfs(); 

  
  //deallocate pointer
  freeifaddrs( initial_ptr );

  return false;
}


template<typename InputType>
void PhyLayer<InputType>::_activate_intfs ( )
{
  
  for(auto& intf : _sm._txrx_socks ) intf.activate();
  //std::ranges::for_each(_sm._txrx_socks, std::identity{}, &socket_data::activate );

  _intfs_ready = std::ranges::any_of(_sm._txrx_socks, std::identity{} );
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

socket_data::socket_data(int intf_idx, std::string src_mac_addr )
{
    intf_index = intf_idx;
    src_mac = src_mac_addr;   
    /*prepare sockaddr_ll*/
    /*RAW communication*/
    socket_addr.sll_family = PF_PACKET;	
    /*we don't use a protocoll above ethernet layer
 *      *   ->just use anything here*/
    socket_addr.sll_protocol = htons(ETH_P_IP);	

    /*index of the network device
 *      * see full code later how to retrieve it*/
    socket_addr.sll_ifindex  = intf_idx;

    /*ARP hardware identifier is ethernet*/
    socket_addr.sll_hatype   = ARPHRD_ETHER;
	
    /*target is another host*/
    socket_addr.sll_pkttype  = PACKET_OTHERHOST;

    /*address length*/
    socket_addr.sll_halen    = ETH_ALEN;		
    /*MAC - end*/
    socket_addr.sll_addr[6]  = 0x80;/*not used*/
    socket_addr.sll_addr[7]  = 0xAB;/*not used*/

}

void socket_data::set_dst( std::string dst_mac )
{
  /*MAC - begin*/
  socket_addr.sll_addr[0]  = dst_mac[0];		
  socket_addr.sll_addr[1]  = dst_mac[1];		
  socket_addr.sll_addr[2]  = dst_mac[2];		
  socket_addr.sll_addr[3]  = dst_mac[3];		
  socket_addr.sll_addr[4]  = dst_mac[4];		
  socket_addr.sll_addr[5]  = dst_mac[5];		

}

void socket_data::activate()
{
  if ( !_tx_socket_fd )
  {
    _tx_socket_fd = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(ETH_P_ALL));
    if( _tx_socket_fd.value() < 0) 
    {
      _tx_socket_fd.reset();
      std::cout << " Could not open transmit socket..." << std::endl;
    }
  }

  if ( !_rx_socket_fd )
  {
    _rx_socket_fd = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(ETH_P_ALL));
    if( _rx_socket_fd.value() < 0) 
    {
      _rx_socket_fd.reset();
      std::cout << " Could not open recv socket..." << std::endl;
    }
  }
}


void socket_data::deactivate()
{

  if ( _tx_socket_fd ){
   _tx_socket_fd.reset();
   close(_tx_socket_fd.value());
  }

  if ( _rx_socket_fd ){ 
    _rx_socket_fd.reset();
    close(_rx_socket_fd.value());
  }

}
