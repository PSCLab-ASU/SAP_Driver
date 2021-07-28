#include <queue>
#include <ranges>
#include <iostream>
#include <optional>
#include <algorithm>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"
#include "include/layers/packets/inout_packet.h"

#pragma once

/* InOut Shared memory section */
struct InOutSM
{


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
        pop = [&]( ushort op )->std::optional<PipelineOutput>
              {
                if( _output_q.empty() ) return {};
                
                PipelineOutput& out = _output_q.front();
                auto& aop = out.get_atomic_op( );

                while( !aop.compare_exchange_weak(op, common_layer_cmds::noop ) )
                  continue;

                std::lock_guard lk(_out_mu);
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
          auto& data = _input_q.front();
          auto op   = data.get_pkt_operation();
          std::cout << "Ingressing packet :  " << std::to_string(op) << std::endl;

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

      PipelineOutput po( in.get_base() );
      po.make_op_atomic();
  
      _output_q.push( std::move(po)  );

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

