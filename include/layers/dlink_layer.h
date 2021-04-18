#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/dlink_packet.h"
#include "include/layers/packets/phy_packet.h"

#pragma once

struct DLinkSM
{

};


template< typename InputType=NullType>
class DatalinkLayer : public base_layer<DatalinkLayer<InputType>, DatalinkPacket >
{
  public: 

    using InputType_t = InputType;
    using DatalinkPktVec = std::vector<DatalinkPacket>;

    using base_layer<DatalinkLayer<InputType>, DatalinkPacket >::get_hb_count;
    using base_layer<DatalinkLayer<InputType>, DatalinkPacket >::is_inited;

    DatalinkLayer();

    template<std::ranges::input_range R>
    DatalinkLayer( R& r ) 
    : DatalinkLayer()
    {
      std::cout << "DatalinkLayer ctor " << std::endl;
    }

    template<std::ranges::input_range R>
    DatalinkLayer( R& r, InputType& in ) 
    : DatalinkLayer()
    {
      std::cout << "DatalinkLayer ctor(in) " << std::endl;
    }

    template<std::ranges::input_range R>
    DatalinkLayer( R& r, NullType in ) 
    : DatalinkLayer()
    {
      std::cout << "Datalink Layer (NullType) " << std::endl;
    }

  private:

    int _noop(DatalinkPacket&& in, DatalinkPktVec& out );
      
    int _self_ds(DatalinkPacket&& in, DatalinkPktVec& out );

    int _self_us(DatalinkPacket&& in, DatalinkPktVec& out );

    int _cleanup_ds(DatalinkPacket&& in, DatalinkPktVec& out );

    int _cleanup_us(DatalinkPacket&& in, DatalinkPktVec& out );

    int _set_mac_addr(DatalinkPacket&& in, DatalinkPktVec& out );

    void _init(  DatalinkPktVec& out );

    void _self_ds_lldp_pkt_gen( DatalinkPktVec& out );

    DatalinkPacket _req_mac_address();

    static DLinkSM _sm;

    const size_t _lldp_interval = 100;
  
};

template class DatalinkLayer<NullType>;
