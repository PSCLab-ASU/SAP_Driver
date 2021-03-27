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

  InOutPacket()
  : BasePacket()
  {}

   //restriction interface view
  InOutPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}

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
      this->template register_cmd<FromApp>(self, &class_type::input_mux);
      this->template register_cmd<FromPhy>(self, &class_type::output_demux);
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
      std::cout << "InOut ctor(T)..." << std::endl;
      auto plf   = std::get<0>( *in );
      auto push  = std::get<1>( *in );
      auto pop   = std::get<2>( *in );

      if( plf == Pipelineflow::FromApp )
      {
        push = [&](PipelineInput&& inp )
                {
                  std::lock_guard lk(_in_mu);
                  _input_q.push( inp );
                };
      }
      else
      {
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
 
    int input_mux(InOutPacket&& in, InOutPktVec& out )
    {
      std::lock_guard lk( _in_mu );
      if( _input_q.empty() ) out.push_back( in );
      else
      {
        while( !_input_q.empty() ){
          out.push_back( _input_q.front().get_data() );
          _input_q.pop();
        }
      }
      return 0;
    }

    int output_demux(InOutPacket&& in, InOutPktVec& out )
    {
      std::lock_guard lk( _out_mu );
      _output_q.push( in.get_base() );

      out.push_back( InOutPacket{} );

      return 0;
    }


    std::mutex _in_mu, _out_mu;

    std::queue<PipelineInput>  _input_q;

    std::queue<PipelineOutput> _output_q;

    inline static InOutSM      _sm;
    
};

