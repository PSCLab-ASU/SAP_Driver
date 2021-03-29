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

  struct ctrl_intf
  {
    friend class NetworkPacket;

    public :
    
      auto get(){ return _ctrl; }

    private:

      ctrl_intf( const std::vector<uchar>& ctrl )
      : _ctrl( ctrl ) 
      {}     
 
      ctrl_intf( ){}

      std::vector<uchar> _ctrl;  
    
  };

  ctrl_intf get_ctrl()
  {
    return ctrl_intf( get_ctrl() );
  }

  static ctrl_intf create_ctrl( )
  {
    return ctrl_intf();
  }

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

    int _cleanup_ds(NetworkPacket&& in, NetworkPktVec& out );

    int _cleanup_us(NetworkPacket&& in, NetworkPktVec& out );

    static NetSM _sm;
};

template class NetworkLayer<NullType>;
