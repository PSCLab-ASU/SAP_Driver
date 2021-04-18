#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/sess_packet.h"
#include "include/layers/packets/trans_packet.h"

#pragma once


struct SessSM
{

};

template< typename InputType=NullType>
class SessionLayer : public base_layer<SessionLayer<InputType>, SessionPacket >
{
  public: 

    using InputType_t = InputType;
    using SessionPktVec = std::vector<SessionPacket>;

    SessionLayer();

    template<std::ranges::input_range R>
    SessionLayer( R& r ) 
    : SessionLayer()
    {
      std::cout << "SessionLayer ctor " << std::endl;
    }

    template<std::ranges::input_range R>
    SessionLayer( R& r, InputType& in ) 
    : SessionLayer()
    {
      std::cout << "SessionLayer ctor(in) " << std::endl;
    }

    template<std::ranges::input_range R>
    SessionLayer( R& r, NullType in ) 
    : SessionLayer()
    {
      std::cout << "Session Layer (NullType) " << std::endl;
    }

  private:

    int _noop(SessionPacket&& in, SessionPktVec& out );
      
    int _self_ds(SessionPacket&& in, SessionPktVec& out );

    int _self_us(SessionPacket&& in, SessionPktVec& out );

    int _cleanup_ds(SessionPacket&& in, SessionPktVec& out );

    int _cleanup_us(SessionPacket&& in, SessionPktVec& out );

    static SessSM _sm;
};

template class SessionLayer<NullType>;
