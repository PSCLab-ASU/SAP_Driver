#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/trans_packet.h"
#include "include/layers/packets/net_packet.h"

#pragma once


struct TransSM
{
  using device_info_reg_t = std::vector<TransportPacket::device_information>;

  bool device_exists( const device_info_reg_t::value_type& ) const;
 
  bool exact_match( const device_info_reg_t::value_type& ) const;
 
  TransportPacket::device_information& 
  get_device_info( const TransportPacket::device_information& dev_info );
  
 
  void add_device_information( const TransportPacket::device_information& dl_dev_info)
  {
    std::lock_guard lk(_devices.first);
    _devices.second.push_back( dl_dev_info );
  }

  std::pair<std::mutex, device_info_reg_t> _devices;

};

template< typename InputType=NullType>
class TransportLayer : public base_layer<TransportLayer<InputType>, TransportPacket >
{
  public: 

    using InputType_t = InputType;
    using TransportPktVec = std::vector<TransportPacket>;

    TransportLayer();

    template<std::ranges::input_range R>
    TransportLayer( R& r ) 
    : TransportLayer()
    {
      std::cout << "TransportLayer ctor " << std::endl;
    }

    template<std::ranges::input_range R>
    TransportLayer( R& r, InputType& in ) 
    : TransportLayer()
    {
      std::cout << "TransportLayer ctor(in) " << std::endl;
    }

    template<std::ranges::input_range R>
    TransportLayer( R& r, NullType in ) 
    : TransportLayer()
    {
      std::cout << "Transport Layer (NullType) " << std::endl;
    }

  private:

    int _noop(TransportPacket&& in, TransportPktVec& out );
      
    int _self_ds(TransportPacket&& in, TransportPktVec& out );

    int _self_us(TransportPacket&& in, TransportPktVec& out );

    int _cleanup_ds(TransportPacket&& in, TransportPktVec& out );

    int _cleanup_us(TransportPacket&& in, TransportPktVec& out );

    int _track_device(TransportPacket&& in, TransportPktVec& out );

    inline static TransSM _sm;
};

template class TransportLayer<NullType>;
