#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/net_packet.h"
#include "include/layers/packets/dlink_packet.h"

#pragma once


struct NetSM
{
  using device_info_reg_t = std::vector<NetworkPacket::device_information>;

  bool device_exists( const device_info_reg_t::value_type& ) const;
 
  bool exact_match( const device_info_reg_t::value_type& ) const;
 
  NetworkPacket::device_information& 
  get_device_info( const NetworkPacket::device_information& dev_info );
  
 
  void add_device_information( const NetworkPacket::device_information& dl_dev_info)
  {
    std::lock_guard lk(_devices.first);
    _devices.second.push_back( dl_dev_info );
  }

  size_t get_num_devices(){
    return _devices.second.size();
  }

  std::pair<std::mutex, device_info_reg_t> _devices;

};


template< typename InputType=NullType>
class NetworkLayer : public base_layer<NetworkLayer<InputType>, NetworkPacket >
{
  public: 

    using InputType_t = InputType;
    using NetworkPktVec = std::vector<NetworkPacket>;

    NetworkLayer();

    template<std::ranges::input_range R>
    NetworkLayer( R& r ) 
    : NetworkLayer()
    {
      std::cout << "NetworkLayer ctor " << std::endl;
    }

    template<std::ranges::input_range R>
    NetworkLayer( R& r, InputType& in ) 
    : NetworkLayer()
    {
      std::cout << "NetworkLayer ctor(in) " << std::endl;
    }

    template<std::ranges::input_range R>
    NetworkLayer( R& r, NullType in ) 
    : NetworkLayer()
    {
      std::cout << "Network Layer (NullType) " << std::endl;
    }

  private:

    int _noop(NetworkPacket&& in, NetworkPktVec& out );
      
    int _self_ds(NetworkPacket&& in, NetworkPktVec& out );

    int _self_us(NetworkPacket&& in, NetworkPktVec& out );

    int _cleanup_ds(NetworkPacket&& in, NetworkPktVec& out );

    int _cleanup_us(NetworkPacket&& in, NetworkPktVec& out );

    int _track_device(NetworkPacket&& in, NetworkPktVec& out );

    int _keep_alive(NetworkPacket&& in, NetworkPktVec& out );

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    NetworkPacket _packetize_discovery( const NetworkPacket::device_information& );

    inline static NetSM _sm;
};

template class NetworkLayer<NullType>;
