#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once
struct DLinkSM
{

};


struct DatalinkPacket : public BasePacket
{
  using BasePacket::get_base;

  struct ctrl_intf
  {
    friend class DatalinkPacket;

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
  DatalinkPacket( typename BasePacket::type base );

  enum : unsigned char { var=common_layer_cmds::END+1 };
};

template< typename InputType=NullType>
class DatalinkLayer : public base_layer<DatalinkLayer<InputType>, DatalinkPacket >
{
  public: 

    using InputType_t = InputType;
    using DatalinkPktVec = std::vector<DatalinkPacket>;

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

    static DLinkSM _sm;
};

template class DatalinkLayer<NullType>;
