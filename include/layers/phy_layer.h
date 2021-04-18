#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/phy_packet.h"

#pragma once

struct PhySM
{

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

    int _send(PhyPacket&& in, PhyPktVec& out );

    int _internal_send(PhyPacket&& in );

    static PhySM _sm;
};

template class PhyLayer<NullType>;
