#include <type_traits>
#include <iostream>
#include <thread>
#include "include/sap_impl.h"
#include "include/utils.h"

SAPLibPImpl::SAPLibPImpl()
{
 
  auto from_app = std::integral_constant<Pipelineflow, Pipelineflow::FromApp>{}; 
  auto from_phy = std::integral_constant<Pipelineflow, Pipelineflow::FromPhy>{}; 
  //kick off pipeline threads
  //_start_pipeline();
  
  //starting forward flowa
  _forward_flow = std::jthread(std::ref(*this), from_app);
  //starting backwards flow 
  _backward_flow = std::jthread(std::ref(*this), from_phy);

}

template<Pipelineflow plf>
void SAPLibPImpl::operator()( std::stop_token stop_token, std::integral_constant<Pipelineflow, plf> )
{
  
  auto pipe = _pipeline.get_pipeline<plf>( );

  std::cout << "Started Pipelines" << std::endl;

  for (auto pkts : pipe )
  {
    _app_cv.notify_one(); //Warm up tte pipeline and release the application
    //if( stop_token.stop_requested() || wait_terminate( pkts, plf ) ){
    if( wait_terminate( pkts, plf ) ){
       std::cout << "Terminating pipeline thread : " <<((ushort)plf) << std::endl;
       break;
    }
  }
}



void SAPLibPImpl::init()
{
  std::unique_lock lk(_app_mu);
  _app_cv.wait(lk);
}


void SAPLibPImpl::finalize()
{

  auto input = PipelineInput(true);

  input.set_pkt_operation( common_layer_cmds::cleanup );

  _pipeline.push( std::move(input) );
 
}


SAPLibPImpl& GetDefaultSAPPImpl()
{
  std::cout << "GetDefaultSAPPImpl ..." << std::endl;
  static SAPLibPImpl lib_impl;
  return lib_impl;
}

