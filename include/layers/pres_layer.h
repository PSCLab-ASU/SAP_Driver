#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/pres_packet.h"
#include "include/layers/packets/sess_packet.h"

#pragma once

struct PresSM
{

};

template< typename InputType=NullType>
class PresentationLayer : public base_layer<PresentationLayer<InputType>, PresentationPacket >
{
  public: 

    using InputType_t = InputType;
    using PresentationPktVec = std::vector<PresentationPacket>;

    PresentationLayer();

    template<std::ranges::input_range R>
    PresentationLayer( R& r ) 
    : PresentationLayer()
    {
      std::cout << "PresentationLayer ctor " << std::endl;
    }

    template<std::ranges::input_range R>
    PresentationLayer( R& r, InputType& in ) 
    : PresentationLayer()
    {
      std::cout << "PresentationLayer ctor(in) " << std::endl;
    }

    template<std::ranges::input_range R>
    PresentationLayer( R& r, NullType in ) 
    : PresentationLayer()
    {
      std::cout << "Presentation Layer (NullType) " << std::endl;
    }

  private:

    int _noop(PresentationPacket&& in, PresentationPktVec& out );
      
    int _self_ds(PresentationPacket&& in, PresentationPktVec& out );

    int _self_us(PresentationPacket&& in, PresentationPktVec& out );

    int _cleanup_ds(PresentationPacket&& in, PresentationPktVec& out );

    int _cleanup_us(PresentationPacket&& in, PresentationPktVec& out );

    int _get_devices(PresentationPacket&& in, PresentationPktVec& out );

    int _func0(PresentationPacket&& in, PresentationPktVec& out );


    static PresSM _sm;
};

template class PresentationLayer<NullType>;
