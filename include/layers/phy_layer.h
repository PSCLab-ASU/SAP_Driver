#include <ranges>
#include <iostream>
#include <optional>
#include <linux/if_packet.h>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/phy_packet.h"

#pragma once

struct socket_data

{
  socket_data(std::string, int , std::string );

  void activate();
  void deactivate();

  void set_dst( std::string );

  operator bool()
  {
    return _tx_socket_fd && _rx_socket_fd;
  }

  int get_rx_socket()
  {
    return _rx_socket_fd.value(); 
  }

  int get_id()
  {
    return intf_index;
  }
  
  int intf_index;
  std::string intf_name;
  std::string src_mac;
  std::optional<int> _tx_socket_fd;
  std::optional<int> _rx_socket_fd;
  sockaddr_ll socket_addr;
};


struct PhySM
{

  void add_sock( std::string ifname, int intf_idx, std::string src_mac )
  {
    _txrx_socks.emplace_back(ifname, intf_idx, src_mac );  
  }

  std::vector<socket_data> _txrx_socks;

};

template< typename InputType=NullType>
class PhyLayer : public base_layer<PhyLayer<InputType>, PhyPacket >
{
  public: 

    using InputType_t = InputType;
    using PhyPktVec = std::vector<PhyPacket>;

    PhyLayer();

    template<std::ranges::input_range R>
    PhyLayer( R& r ) 
    : PhyLayer()
    {
      std::cout << "PhyLayer ctor " << std::endl;
      std::cout << "enum set_intfs value : " << std::to_string(PhyPacket::set_intfs) << std::endl;
    }

    template<std::ranges::input_range R>
    PhyLayer( R& r, InputType& in ) 
    : PhyLayer()
    {
      std::cout << "PhyLayer ctor(in) " << std::endl;
    }

    template<std::ranges::input_range R>
    PhyLayer( R& r, NullType in ) 
    : PhyLayer()
    {
      std::cout << "Phy Layer (NullType) " << std::endl;
    }

  private:

    int _noop(PhyPacket&& in, PhyPktVec& out );
      
    int _self_ds(PhyPacket&& in, PhyPktVec& out );

    int _self_us(PhyPacket&& in, PhyPktVec& out );

    int _cleanup_ds(PhyPacket&& in, PhyPktVec& out );

    int _cleanup_us(PhyPacket&& in, PhyPktVec& out );

    int _get_mac(PhyPacket&& in, PhyPktVec& out );

    int _set_intfs(PhyPacket&& in, PhyPktVec& out );

    int _send(PhyPacket&& in, PhyPktVec& out );


    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    int _internal_send(PhyPacket&& in );

    bool _configure_interfaces ( std::vector<std::string>  );

    void _activate_intfs ( );

    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    inline static PhySM _sm;
    bool _intfs_ready = false;
};

template class PhyLayer<NullType>;

