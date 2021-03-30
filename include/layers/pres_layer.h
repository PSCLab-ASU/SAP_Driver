#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once
struct PresSM
{

};

struct PresentationPacket : public BasePacket
{
  using BasePacket::get_base;

  struct ctrl_intf
  {
    friend class PresentationPacket;

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
  PresentationPacket( typename BasePacket::type base );

  enum : unsigned char { var=common_layer_cmds::END+1 };

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

    int _func0(PresentationPacket&& in, PresentationPktVec& out );

    int _func1(PresentationPacket&& in, PresentationPktVec& out );

    static PresSM _sm;
};

template class PresentationLayer<NullType>;
