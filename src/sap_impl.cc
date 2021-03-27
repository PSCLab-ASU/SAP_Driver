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
  std::cout << "SAPLibPImpl ctor start" << std::endl;
  
  //auto pipe = _pipeline.get_pipeline<Pipelineflow::FromApp>( );

  //std::cout << "completed pipe creation " << std::endl;
  //for (auto stat : pipe )
  //  std::cout << "stat :: " << std::endl;
  
  //starting forward flowa
  _forward_flow = std::jthread(std::ref(*this), from_app);
  //starting backwards flow 
  _backward_flow = std::jthread(std::ref(*this), from_phy);
  
  std::cout << "SAPLibPImpl ctor end " << std::endl;

}

template<Pipelineflow plf>
void SAPLibPImpl::operator()( std::stop_token stop_token, std::integral_constant<Pipelineflow, plf> )
{
  
  std::cout << "Starting Thread : " << ((ushort)plf) << std::endl;

  auto pipe = _pipeline.get_pipeline<plf>( );

  for (auto stat : pipe ) if( stop_token.stop_requested() ) break;
  
}

void SAPLibPImpl::init()
{


}


void SAPLibPImpl::finalize()
{


}


void SAPLibPImpl::_confirm_pipeline_status( Stat status )
{
  std::cout << "_confirm_pipeline_status" << std::endl;

}


SAPLibPImpl& GetDefaultSAPPImpl()
{
  std::cout << "GetDefaultSAPPImpl ..." << std::endl;
  static SAPLibPImpl lib_impl;
  return lib_impl;
}

