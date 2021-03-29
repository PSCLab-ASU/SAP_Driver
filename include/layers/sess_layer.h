#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once
struct SessSM
{

};


struct SessionPacket : public BasePacket
{
  using BasePacket::get_base;

  struct ctrl_intf
  {
    friend class SessionPacket;

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
  SessionPacket( typename BasePacket::type base );

  enum : unsigned char { var=common_layer_cmds::END+1 };
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
