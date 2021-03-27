#include <type_traits>
#include <optional>
#include <thread>
#include <ranges>

#include "include/utils.h"
#include "include/pipeline.h"
#include "include/pipeline_data.h"


class SAPLibPImpl;

SAPLibPImpl& GetDefaultSAPPImpl();

class SAPLibPImpl
{
  
  public :
  
    void init(); 

    void finalize(); 

    template<Pipelineflow plf >
    void operator()( std::stop_token, std::integral_constant<Pipelineflow, plf> );

  private :

    friend SAPLibPImpl& GetDefaultSAPPImpl();

    SAPLibPImpl();


    void _confirm_pipeline_status( Stat );

    pipeline_impl _pipeline;

    std::jthread _forward_flow;
    std::jthread _backward_flow;

};


