#include <queue>
#include <ranges>
#include <iostream>
#include <optional>
#include <algorithm>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once

/* InOut Shared memory section */
struct InOutSM
{


};

/* Custom packet interface for each layer helper */
struct InOutPacket : public BasePacket
{
  using BasePacket::get_base;

  struct ctrl_intf
  {
    friend class InOutPacket;

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

  InOutPacket()
  : BasePacket()
  {}

   //restriction interface view
  InOutPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}
 
  enum : unsigned char { output=common_layer_cmds::END+1 };
};

/* main layer defintion */
template< typename InOutType=NullType>
class InOutLayer : public base_layer<InOutLayer<InOutType>, InOutPacket >
{
  public: 
    using InputType_t = NullType;
    using InOutPktVec = std::vector<InOutPacket>;

    InOutLayer()
    {
      using namespace common_layer_cmds;
      using class_type = InOutLayer<InOutType>;

      const auto FromApp = Pipelineflow::FromApp;
      const auto FromPhy = Pipelineflow::FromPhy;
      
  
      std::cout << "0)Registrating InOutLayer functions" << std::endl;
      this->template register_cmd<FromApp>(noop, &class_type::_noop_ds);
      this->template register_cmd<FromPhy>(noop, &class_type::_noop_us);
      this->template register_cmd<FromApp>(cleanup, &class_type::_cleanup_ds);
      this->template register_cmd<FromPhy>(cleanup, &class_type::_cleanup_us);
      /////////////////////////////////////////////////////////////////////////////////////
      this->template register_cmd<FromApp>(self, &class_type::_input_mux );
      this->template register_cmd<FromPhy>(InOutPacket::output, &class_type::_output_demux);
    }

    template<std::ranges::input_range R>
    InOutLayer( R& r ) 
    : InOutLayer()
    {
      std::cout << "InOutLayer ctor " << std::endl;
    }

    template<std::ranges::input_range R, typename T> 
    InOutLayer(R& r, T in ) 
    : InOutLayer( ) 
    {
      auto  plf   = std::get<0>( *in );
      auto& push  = std::get<1>( *in );
      auto& pop   = std::get<2>( *in );

      if( plf == Pipelineflow::FromApp )
      {
        std::cout << "Adding push interface into pipeline " << std::endl;
        push = [&](PipelineInput&& inp )
                {
                  std::lock_guard lk(_in_mu);
                  std::cout << "Ingressing data from application" << std::endl;
                  _input_q.push( inp );
                };
      }
      else
      {
        std::cout << "Adding pop interface out of pipeline " << std::endl;
        pop = [&]()->std::optional<PipelineOutput>
              {
                std::lock_guard lk(_out_mu);
                if( _output_q.empty() ) return {};
       
                PipelineOutput out = _output_q.front();
                _output_q.pop();
                return out;
              };

      }
      
    }

    template<std::ranges::input_range R> 
    InOutLayer(R& r, NullType inout ) 
    : InOutLayer( ) 
    {
      std::cout << "InOut ctor(Null)..." << std::endl;
    }

  private:
     
    int _noop_ds( InOutPacket&& in, InOutPktVec& out ) 
    {
      std::cout << "noop_ds..." << std::endl;  
      return 0;
    }

    int _noop_us( InOutPacket&& in, InOutPktVec& out ) 
    {
      std::cout << "noop_us..." << std::endl;  
      return 0;
    }
 
    int _input_mux(InOutPacket&& in, InOutPktVec& out )
    {
      std::cout << "self input_mux... " << std::endl;
      std::lock_guard lk( _in_mu );
      std::cout << " after lock... " << std::endl;
      if( _input_q.empty() ) {
        std::cout << "forwarding empty() packet " << std::endl;
        //out.push_back( in );
      }
      else
      {
        std::cout << "Pushing data into Q" << std::endl;
        while( !_input_q.empty() )
        {
          std::cout << "Ingressing packet " << std::endl;
          auto& data = _input_q.front();
          auto op   = data.get_pkt_operation();

          if( op == common_layer_cmds::cleanup )
          {
            std::cout << "Found cleanup command" << std::endl;
            _cleanup_ds( _input_q.front().get_data(), out );          
          }
          //append cleanup packet to downstream
          out.push_back( data.get_data() );

          _input_q.pop();
        }
      }
      return 0;
    }

    int _output_demux(InOutPacket&& in, InOutPktVec& out )
    {
      std::cout << "self output_mux... " << std::endl;
      std::lock_guard lk( _out_mu );
      _output_q.push( in.get_base() );

      out.push_back( InOutPacket{} );

      return 0;
    }

    int _cleanup_ds(InOutPacket&& in, InOutPktVec& out )
    {
      std::cout << "cleanup_ds input_mux... " << std::endl;
      return 0;
    }

    int _cleanup_us(InOutPacket&& in, InOutPktVec& out )
    {
      std::cout << "cleanup_us output_mux... " << std::endl;
      return 0;
    }

    std::mutex _in_mu, _out_mu;

    std::queue<PipelineInput>  _input_q;

    std::queue<PipelineOutput> _output_q;

    inline static InOutSM      _sm;
    
};

