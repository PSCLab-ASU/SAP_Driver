#include <ranges>
#include <iostream>
#include <queue>
#include <algorithm>
#include <type_traits>
#include "include/utils.h"
#include "include/pipeline_data.h"
#include "include/layers/layer_cmd_processor.h"


#pragma once

template<typename Derived, typename DataIntf >
class base_layer  
{
  public : 

    template <typename Range>
    struct layer_iterator {
      using input_type = typename std::ranges::iterator_t<Range>::value_type;
      using conv_type  = typename Derived::InputType_t;
      using value_type = std::conditional_t<std::is_same_v<conv_type, NullType>, input_type, conv_type>;
      using difference_type = std::iter_difference_t<std::ranges::iterator_t<Range>>;
    

      constexpr layer_iterator() = default;
      constexpr layer_iterator(Range& r, auto impl, bool start_end)  
      : _r_ptr(&r), _l_ptr(impl), _it( std::begin(r) ), _start_end(start_end)
      {
        if( start_end )
        {
          std::cout << "Ctor for layer_iterator (start)" << std::endl;
          _offset = 0;
          std::cout << "processing input packets... " << r.size() << std::endl;
          _l_ptr->process_packets( *_it, _egress_queue);
          std::cout << "egress queue size = " << _egress_queue.size() << std::endl;

        }
        else 
        {
          std::cout << "Ctor for layer_iterator (end)" << std::endl;
          _offset = r.size();
        }
         
      }

      layer_iterator<Range>& operator++() 
      {
        std::vector<value_type> _pregress_queue;
        //increment offset
        _offset++;

        //get the input from previous layer
        auto input = *(++_it);
        // Process the packet 

        _l_ptr->process_packets( input, _pregress_queue);
        std::cout << "layer_iterator ++operator : " << _pregress_queue.size() << std::endl;

        std::cout << "moving pregress_queue to eqgress " << std::endl;
        _egress_queue.pop_back();
 
        _egress_queue.insert(_egress_queue.begin(), 
                             _pregress_queue.rbegin(), 
                             _pregress_queue.rend() );       
        //_egress_queue = std::move( _pregress_queue );
        std::cout << "egress size : " << _egress_queue.size() << std::endl;
        return *this; 
      }

      layer_iterator<Range> operator++(int) {
          std::cout << "layer_iter Calling post unary ++ " << std::endl;
          const layer_iterator<Range> current{*this};
          ++(*this);
          return current;
      }

      value_type operator*() const {
          std::cout << "layer_iterator dereference op() : " << _egress_queue.size() << std::endl;
          return _egress_queue.front();
      }

      bool operator==(layer_iterator<Range> const& rhs) const 
      { 
        std::cout << "Are iterators equal " << _offset  << " == " << rhs._offset << std::endl; 
        return (_offset == rhs._offset ); 
      }

  private:
      std::ranges::iterator_t<Range> _it;
      Range *  _r_ptr;
      Derived * _l_ptr;
      bool   _start_end;
      size_t _offset = 0; 
      decltype(std::declval<Range>().begin() ) _iter;
      std::vector<value_type> _egress_queue;
  };
 
    constexpr base_layer( )
    {
      _downstream_cmd_proc.swap_queue_intf( _upstream_cmd_proc );
    }

    template<std::ranges::input_range R>
    //auto begin(R& r ) const
    layer_iterator<R> begin(R& r )
    {
      std::cout << "Calling base_layer begin()" << std::endl;
      return layer_iterator<R>(r, _Impl(), true );
    }

    template<std::ranges::input_range R>
    //auto end(R& r ) const
    auto end(R& r )
    {
      std::cout << "Calling base_layer end()" << std::endl;
      return layer_iterator<R>(r, _Impl(), false );
    }

    template<typename R, typename Q>
    void process_packets(R input_vec, Q& output_vec)
    {
      if constexpr( std::is_same_v< R, std::vector<PipelineInput> > )
      {
        // calc layer
        std::cout << "Detected intermediate (downstream) layer" << std::endl;
        _downstream_cmd_proc( input_vec, output_vec);
        std::cout <<" Downstream cmd proc : " << output_vec.size() << std::endl;
        
      }
      else if constexpr( std::is_same_v< R, std::vector<PipelineOutput> > )
      {
        std::cout << "Detected intermediate (upstream) layer" << std::endl;
        _upstream_cmd_proc( input_vec, output_vec);

      }
      else
      {
        //int i = R_type{};
        std::cout << "Unrecognized input type" << std::endl;
      }


    }

    template<Pipelineflow plf, typename Func>
    void register_cmd(uchar cmd_id, Func&& func)
    {

      auto entry = std::bind(func, _Impl(), std::placeholders::_1, std::placeholders::_2); 

      if constexpr( plf == Pipelineflow::FromApp)
        _downstream_cmd_proc.register_function( cmd_id, std::move(entry) );
      else
        _upstream_cmd_proc.register_function( cmd_id, std::move(entry) );
      
    }

  
    Derived * _Impl() { return static_cast<Derived*>(this); } 

    private :

      inline static layer_cmd_processor<Pipelineflow::FromApp, DataIntf, Derived> _downstream_cmd_proc;
      inline static layer_cmd_processor<Pipelineflow::FromPhy, DataIntf, Derived> _upstream_cmd_proc;
   
};


