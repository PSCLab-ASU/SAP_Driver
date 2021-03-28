#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once
struct NetSM
{

};


struct NetworkPacket : public BasePacket
{
  using BasePacket::get_base;
   //restriction interface view
  NetworkPacket( typename BasePacket::type base );

  enum : unsigned char { var=common_layer_cmds::END+1 };
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

    int _func0(NetworkPacket&& in, NetworkPktVec& out );

    int _func1(NetworkPacket&& in, NetworkPktVec& out );

    static NetSM _sm;
};

template class NetworkLayer<NullType>;
