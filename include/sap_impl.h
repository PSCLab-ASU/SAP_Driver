#include <condition_variable>
#include <type_traits>
#include <optional>
#include <thread>
#include <ranges>
#include <mutex>
#include <vector>
#include "include/utils.h"
#include "include/pipeline.h"
#include "include/pipeline_data.h"


class SAPLibPImpl;

SAPLibPImpl& GetDefaultSAPPImpl();

class SAPLibPImpl
{
  
  public :
  
    void init( std::vector<std::string> ); 

    void finalize(); 

    template<Pipelineflow plf >
    void operator()( std::stop_token, std::integral_constant<Pipelineflow, plf> );

  private :

    friend SAPLibPImpl& GetDefaultSAPPImpl();

    SAPLibPImpl();

    bool wait_terminate( auto& packets, auto plf )
    {  
      bool is_cleanup = std::ranges::any_of( packets, [](auto pkt)
                        {
                          return (pkt.get_pkt_operation() ==
                                  common_layer_cmds::cleanup );
                        } );
      
      if( (plf == Pipelineflow::FromApp) && is_cleanup )
      {
        std::unique_lock<std::mutex> lk(_mu);
        std::cout << "Freezing downstream pipeline..." << std::endl;
        _cv.wait(lk);
        return true;
      }
      else if( (plf == Pipelineflow::FromPhy) && is_cleanup )
      {
        _cv.notify_one();  
        return true;
      }
 
      return false;
    }

    pipeline_impl _pipeline;

    std::mutex _mu, _app_mu;
    std::condition_variable _cv, _app_cv;
    std::jthread _forward_flow;
    std::jthread _backward_flow;

};


