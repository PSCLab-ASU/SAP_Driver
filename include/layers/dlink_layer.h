#include <mutex>
#include <ranges>
#include <iostream>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/dlink_packet.h"
#include "include/layers/packets/phy_packet.h"

#pragma once

struct DLinkSM
{
  using device_info_reg_t = std::vector<DatalinkPacket::device_information>;

  bool device_exists( const device_info_reg_t::value_type&, bool& ) const;
 
  DatalinkPacket::device_information& 
  get_device_info( const DatalinkPacket::device_information& dev_info );
  
  DatalinkPacket::device_information& 
  get_device_by_src_mac( std::string );
 
  void add_device_information( const DatalinkPacket::device_information& dl_dev_info)
  {
    std::lock_guard lk(_dev_mu);
    _devices.push_back( dl_dev_info );
  }
 
  DatalinkPacket::device_information&
  get_last_dev_info()
  { 
    return _devices.at(_devices.size()-1 );
  }

  DatalinkPacket::device_information& 
  update_device_status( int, std::string, std::optional<bool>,
                        std::optional<accel_descriptor>  );

  mutable std::mutex _dev_mu;
  device_info_reg_t  _devices;
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

    int _track_device(DatalinkPacket&& in, DatalinkPktVec& out );

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    void _init(  DatalinkPktVec& out );

    void _check_invalidate( DatalinkPktVec& out );

    template<typename DevUpdate=NullType>
    void _upstream_dev_info( DatalinkPacket& prev, DatalinkPktVec& out, std::optional<DevUpdate> dev_info ={} );

    DatalinkPacket _gen_exp_mac_req( std::vector<std::string>&& );

    DatalinkPacket _req_mac_address();

    inline static DLinkSM _sm;

    const size_t _device_timeout = 60;

    const size_t _lldp_interval = 100;
  
};

template class DatalinkLayer<NullType>;
