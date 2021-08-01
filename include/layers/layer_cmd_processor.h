#include <map>
#include <tuple>
#include <queue>
#include <mutex>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <functional>
#include "include/pipeline_data.h"

#pragma once

template<Pipelineflow plf, typename DataIntf, typename DerivedTag>
class layer_cmd_processor
{

  public:

    using func_out_parm = typename std::vector<DataIntf>;
    using typed_data    = typename std::vector<pipeline_data<plf> >; 
    using func_decl     = std::function<int(DataIntf&&, func_out_parm&)>;


    layer_cmd_processor() {};

    void register_function( uint key, func_decl&& func) 
    {
      _command_processor.insert({key, func});  
    }
  
    int operator() (typed_data& input, std::vector<typed_data>& output ) 
    { 
      typed_data temp_out;
   
      //takes (processes) a single vector element from the previous layer
      std::ranges::for_each( input, [&](auto& in){ _process_single(in, temp_out);  }  ); 

      //takes queue packets and processes all othe them
      while( !_other_q_empty() ) 
      { 
        auto q_element = _other_q_pop();

        std::ranges::for_each( q_element, [&](auto& in){ 
          printf("CROSS Q Reading.... op : %i \n", 
                in.get_pkt_operation() );
                in.reset_dst();
                _process_single<false, true>(in, temp_out);  }  ); 
          //output.push_back( q_element );
      }
      //adding current layer packets
      typename typed_data::value_type empty;
      _process_single<true>(empty, temp_out);   

      //process all output to final queues
      _output_data( input, std::move(temp_out), output );

      return 0;
    }

   
    template<typename T>
    void swap_queue_intf( T& cp )
    {
      set_queue_rxintf( cp.template get_queue_rxintf<typename typed_data::value_type>() );
      cp.set_queue_rxintf( get_queue_rxintf<typename T::typed_data::value_type>() );
    }

    template<typename TransFormT >
    auto get_queue_rxintf()
    {
      return _get_queue_rxintf<TransFormT>();
    }

    template<typename T>
    void set_queue_rxintf( T&& intf )
    {
      _set_queue_rxintf( std::forward<T>( intf ) );
    }
 
  private: 
 
    void _output_data( auto& input, auto&& processed_data, auto& output )
    {
      if( processed_data.empty() )
      {
        /*std::ranges::for_each( input, [](auto& in)
        {
          printf( "Forwarding INPUT with op %i \n", in.get_pkt_operation() );
        });*/

        output.push_back( std::move(input) ); 
      }
      else if( std::ranges::none_of( processed_data, 
                                    std::negate{}, 
                                    &typed_data::value_type::get_dst )  ) 
      {
        std::cout << "All goes downstream..." << std::endl;
        output.push_back( std::move(processed_data) );
      }
      else if( std::ranges::all_of( processed_data, 
                                    std::negate{}, 
                                    &typed_data::value_type::get_dst )  ) 
      {
        std::cout << "All goes queue..." << std::endl;       
        std::ranges::for_each(processed_data, [](auto& in){ in.reset_dst(); } );

        _write_data_to_q( std::move(processed_data) );

        auto cleanup_it = std::ranges::find(input, common_layer_cmds::cleanup, 
                                            &typed_data::value_type::get_pkt_operation);

        //adding empty downstream packet to continue heart beat
        //if( cleanup_it != std::end(processed_data) )
        if( cleanup_it != std::end(input) )
        {
          std::cout << "Found cleanup token in the output" << std::endl;
          output.push_back( {*cleanup_it} );
        }
        else
          output.push_back( typed_data{} );
        
      }
      else
      {
        std::cout << "All goes to mixed channels" << std::endl;
        //means thier are a mixture of things going to the queue and then the output
        auto it = std::ranges::partition( processed_data, std::negate{},
                                          &typed_data::value_type::get_dst );
        
        typed_data layer_data, q_data;
        //moving elements to next layer
        std::ranges::move(it, std::back_inserter(layer_data) );
        output.push_back( std::move( layer_data ) ); 
        
        //moving elements to the queue
        std::ranges::move(std::begin(processed_data), 
                          std::begin(it), std::back_inserter(q_data) );

        std::ranges::for_each(q_data, [](auto& in){ in.reset_dst(); } );

        _write_data_to_q( std::move(q_data) );
        
      } 

    }

    template<bool Override=false, bool PassThru=false>
    void _process_single (typename typed_data::value_type& in, typed_data& out)
    {
      //process input
      func_out_parm temp;   
      
      if( PassThru ) {
        temp.push_back(in.get_data() );
        std::ranges::transform(temp, std::back_inserter(out), &DataIntf::get_base );
        return;
      }

 
      if( in || Override )
      {
        //get the operation
        auto op = Override?common_layer_cmds::self:in.get_pkt_operation();

        if( auto cmd = _command_processor.find(op); cmd != _command_processor.end() )
        {
          cmd->second( in.get_data(), temp );
          //transform to out type
          std::ranges::transform(temp, std::back_inserter(out), &DataIntf::get_base );
        }
        else if( auto cmd = _command_processor.find(common_layer_cmds::any ); 
                 cmd != _command_processor.end() )
        {
          //if ( op != common_layer_cmds::self )
          {
            printf("2) Executing ANY procedure... op : %i \n", (ushort) op );
            cmd->second(in.get_data(), temp ); 
          }
        }

        if( op == common_layer_cmds::cleanup )
        {
          std::cout << "Forwarding cleanup packet" << std::endl;
          out.push_back( in.get_data() );        
        }
      }
       
    }

    template<typename TransFormT >
    auto _get_queue_rxintf()
    {
       auto is_empty_intf = [&]()->bool
       {
         const std::lock_guard<std::mutex> lock(_q_mu);

         return  _packet_q.empty();
  
       };

       std::function<std::vector<TransFormT>()> pop_element = [&]() -> std::vector<TransFormT>
       {
         const std::lock_guard<std::mutex> lock(_q_mu);

         typed_data element = _packet_q.front();
         std::vector<TransFormT> out;
  
         std::ranges::transform(element, std::back_inserter(out), 
                               [](auto in) { return (TransFormT)in; } );
          
         _packet_q.pop();
         
         return  out;
       };

       auto intf = std::make_pair( is_empty_intf, pop_element );

       return intf;

    }

    void _set_queue_rxintf( auto&& intf )
    {
      _other_q_empty = intf.first;
      _other_q_pop   = intf.second;
      
    }

    void _write_data_to_q( auto&& data )
    {
      std::lock_guard lock(_q_mu);
      _packet_q.push( std::forward<decltype(data)>(data) );        
    }

    std::function<bool()> _other_q_empty;
    std::function<typed_data()> _other_q_pop;

    std::mutex _q_mu;
    std::queue<typed_data> _packet_q;   

    std::map<uchar, func_decl> _command_processor;
};

