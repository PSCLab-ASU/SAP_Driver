#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <linux/udp.h>
#include <net/if.h>
#include <arpa/inet.h>
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
    std::cout << "Reading from " << active_intf.intf_name <<
              "(" << active_intf.get_rx_socket() << ")" <<  std::endl;
    auto bytes = recv(active_intf.get_rx_socket(), data, 
                      MAX_MTU, MSG_DONTWAIT ); 

    if( bytes > 0 )
    {
      ether_header * eh   = (ether_header *) data;

      auto src_mac = std::string((const char*) eh->ether_shost,  6);
      auto dst_mac = std::string((const char*) eh->ether_dhost,  6);

      printf("ether_type = %04x, dst = %02x:%02x:%02x:%02x:%02x:%02x\n", eh->ether_type,
             dst_mac[0], dst_mac[1], dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5] );

      if( eh->ether_type == g_accelerator_type )
      {
        printf(" FOUND SAP Packet : src %02x:%02x:%02x:%02x:%02x:%02x!!!\n",
               eh->ether_shost[0],eh->ether_shost[1],eh->ether_shost[2],
               eh->ether_shost[3],eh->ether_shost[4],eh->ether_shost[5]);

        PhyPacket p; 
   
        p.set_intf_id( active_intf.get_id() );
        p.allocate_data( bytes );

        p.set_src( src_mac );   
        p.set_offsets(0, ETH_H_LEN);

        auto& vec = p.get_data<false>();

        for(int i=0; i < bytes; i++)
        {
          vec[i] = (0xFF & data[i]); 
          //printf("%i:%02x:%02x\t", i, vec[i], data[i] );
          //if( (i%12) == 0) printf("\n");
        }
   
        p.parse_ctrl();
 
        if( p.get_op() == 1 ) p.set_op( common_layer_cmds::discovery );
        
        out.push_back( p );
      }
      else{
        std::cout << "Not a SAP packet.... : " << eh->ether_type << std::endl;
        bytes = recv(active_intf.get_rx_socket(), data, MAX_MTU, MSG_DONTWAIT ); 
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
  printf("PHY2 entering _set_intfs \n");
  std::vector<std::string> intfs;
  auto[intf_sz, n_intfs, intf_ptr] = in.get_tlv(0);

  std::string intf_name = std::string( (char *) intf_ptr, intf_sz);
  printf("PHY2 _set_intfs : intf_sz = %i, n_intfs = %i, name =%s \n", intf_sz, n_intfs, intf_name.c_str() ); 

  intfs.push_back(intf_name);

  if( (intf_sz > 0) && ( n_intfs > 0 ) )
  {
    //TBD COnfigure sockets
    bool can_configure = _configure_interfaces( intfs );

    if ( can_configure ){
      _activate_intfs(); 
    }
    else{
      std::cout << "**************************************" << std::endl;
      std::cout << "Could not configure any interfaces!!" << std::endl;
      std::cout << "**************************************" << std::endl;
    }
  }
  else{
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
  bool found_atleast_one = false;
  std::vector<std::string> found_intfs;

  int res = getifaddrs( &pifap );

  while( pifap != NULL )
  {
    std::cout << "Interface Name : " << pifap->ifa_name << std::endl;

    //find intferace
    bool match = std::ranges::any_of(intfs, [&]( auto intf){
      return intf == std::string( pifap->ifa_name );
    }); 
  
    //auto already_conf = std::ranges::find(found_intfs, std::string(pifap->ifa_name) );
    bool already_conf = std::ranges::find(found_intfs, pifap->ifa_name ) != std::end(found_intfs);
    if( match && !already_conf )
    {
      auto ifname = std::string(pifap->ifa_name);
      found_intfs.push_back(ifname);
      std::cout << "Configuring interface : "<< ifname << std::endl;
      std::string src;
      struct sockaddr_ll *s = (struct sockaddr_ll*)pifap->ifa_addr;

      //pull mac address
      for(int i = 0; i < 6; i++) src.push_back( s->sll_addr[i] );

      _sm.add_sock(ifname, ((sockaddr_ll *)pifap->ifa_addr)->sll_ifindex, src); 
      found_atleast_one = true;
    }

    pifap = pifap->ifa_next;
  }

  //at this point socket handles wil lbe created
  _activate_intfs(); 

  
  //deallocate pointer
  freeifaddrs( initial_ptr );

  return found_atleast_one;
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

socket_data::socket_data(std::string ifname, int intf_idx, std::string src_mac_addr )
{
    intf_name  = ifname;
    intf_index = intf_idx;
    src_mac = src_mac_addr;   
    /*prepare sockaddr_ll*/
    /*RAW communication*/
    socket_addr.sll_family = AF_PACKET;	
    /*we don't use a protocoll above ethernet layer
 *      *   ->just use anything here*/
    socket_addr.sll_protocol = htons(ETH_P_ALL);	

    /*index of the network device
 *      * see full code later how to retrieve it*/
    socket_addr.sll_ifindex  = intf_idx;

    /*ARP hardware identifier is ethernet*/
    //socket_addr.sll_hatype   = ARPHRD_ETHER;
	
    /*target is another host*/
    socket_addr.sll_pkttype  = PACKET_OTHERHOST;

    /*address length*/
    socket_addr.sll_halen    = ETH_ALEN;		
    /*MAC - end*/
    socket_addr.sll_addr[6]  = 0xAB;/*not used*/
    socket_addr.sll_addr[7]  = 0x80;/*not used*/

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
    std::cout << "Activating TX for " << intf_name << std::endl;
    int sockfd = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(ETH_P_ALL));
    _tx_socket_fd = sockfd;

    if( sockfd < 0) 
    {
      _tx_socket_fd.reset();
      std::cout << " Could not open transmit socket..." << std::endl;
    }
  }

  if ( !_rx_socket_fd )
  {
    std::cout << "Activating RX for " << intf_name << std::endl;
    int sockfd = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(ETH_P_ALL));
    _rx_socket_fd = sockfd;
    if( sockfd < 0) 
    {
      _rx_socket_fd.reset();
      std::cout << " Could not open recv socket..." << std::endl;
    }
    else
    {
      std::cout << " configuring " << intf_name << "(" << sockfd << ") permiscuous mode! " << std::endl;
      ifreq ifopts;
      int sockopt;

      strncpy(ifopts.ifr_name, intf_name.c_str(), intf_name.size() );
      ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
      ifopts.ifr_flags |= IFF_PROMISC;
      ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
      /* Allow the socket to be reused - incase connection is closed prematurely */
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
        std::cout << " Error reusing socket to " << intf_name << std::endl;
        _rx_socket_fd.reset();
      }
      /* Bind to device */
      if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, intf_name.c_str(), intf_name.size() ) == -1)
      {
        std::cout << " Error Binding socket to " << intf_name << std::endl;
        _rx_socket_fd.reset();
      }
     
      int broadcast = 1;
      if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int) ) == -1)
      {
        std::cout << " Error Binding broadcast socket to " << intf_name << std::endl;
        _rx_socket_fd.reset();
      }     

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
