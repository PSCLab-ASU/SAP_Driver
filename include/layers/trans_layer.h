#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once
struct TransSM
{

};


struct TransportPacket : public BasePacket
{
  using BasePacket::get_base;
   //restriction interface view
  TransportPacket( typename BasePacket::type base );

  enum : unsigned char { var=common_layer_cmds::END+1 };
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

    static TransSM _sm;
};

template class TransportLayer<NullType>;
