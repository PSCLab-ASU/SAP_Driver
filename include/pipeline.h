#include <ranges>
#include <tuple>
#include "include/pipeline_data.h"
#include "include/layers/layers.h"


#pragma once

using namespace views;

struct pipeline_impl
{

  using InputType = PipelineInput;
  using OutputType = std::optional<PipelineOutput>;

  using InOutIntf = std::tuple<Pipelineflow,
                               std::function<void(InputType&&)>, 
                               std::function<OutputType()> >;

  using _pl_definition = std::tuple<decltype(InOutL() ),
                                    decltype(PresL()()), decltype(SessL()()), decltype(TransL()()), 
                                    decltype(NetL()()), decltype(DataL()()), decltype(PhyL()()) >;
 
  pipeline_impl()
  {
    std::cout << "pipeline_impl ..." << std::endl;

  }
  

  template<Pipelineflow plf, size_t ... Is >
  auto _init( std::index_sequence<Is...> )
  {
    constexpr size_t N = sizeof...(Is);

    auto sleepy_hb = std::ranges::views::iota(1,10) | 
                     std::ranges::views::transform([](int in)
                     {
                       std::this_thread::sleep_for( 1000ms );
                       return in;
                     } );

    auto down_input_conv = std::ranges::views::transform([](int in )->std::vector<InputType>{ return {in}; } ); 
    auto up_input_conv   = std::ranges::views::transform([](int in )->std::vector<typename OutputType::value_type>{ return {in};} ); 

    if constexpr( plf == Pipelineflow::FromApp)
    {
      std::get<0>(_inout_intf) = Pipelineflow::FromApp;
      auto build_pipeline = (( (std::move(sleepy_hb) | down_input_conv) | std::get<0>(_pipeline)(&_inout_intf)  ) | ... | std::get<Is+1>(_pipeline) ); 

      return build_pipeline;
    }
    else
    {
      std::get<0>(_inout_intf) = Pipelineflow::FromPhy;
      auto build_pipeline = (( std::move(sleepy_hb) | up_input_conv ) | ... | std::get<N-Is>(_pipeline) ) | std::get<0>(_pipeline)(&_inout_intf); 
      return build_pipeline;
    }
  }

  template<Pipelineflow plf, size_t N = (std::tuple_size_v<_pl_definition>-1), 
           typename Indices = std::make_index_sequence<N> >
  auto get_pipeline( )
  {
    return _init<plf>( Indices{} );
  }

  void push( InputType&& in )
  {
    auto _push = std::get<1>(_inout_intf);
    _push( std::forward<InputType>(in) );
  }

  OutputType pop()
  {
    auto _pop = std::get<2>(_inout_intf);
    return _pop();
  }

  InOutIntf  _inout_intf;

  _pl_definition  _pipeline;
  
};
