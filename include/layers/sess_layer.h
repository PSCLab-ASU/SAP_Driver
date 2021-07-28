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
  using device_info_reg_t = std::vector<SessionPacket::device_information>;

  bool device_exists( std::string );
 
  SessionPacket::device_information& 
  get_device_info( std::string  );
 
  std::map<std::string, std::string>
  serialize_device_info(); 
 
  void add_device_information( const SessionPacket::device_information& dl_dev_info)
  {
    std::lock_guard lk(_devices.first);
    _devices.second.push_back( dl_dev_info );
  }

  std::pair<std::mutex, device_info_reg_t> _devices;

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

    int _deactivate_device(SessionPacket&& in, SessionPktVec& out );

    int _device_info(SessionPacket&& in, SessionPktVec& out );

    int _get_devices(SessionPacket&& in, SessionPktVec& out );

    inline static SessSM _sm;
};

template class SessionLayer<NullType>;
