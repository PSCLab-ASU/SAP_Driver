#include <type_traits>
#include <iostream>
#include <thread>
#include "include/sap_impl.h"
#include "include/utils.h"
#include "include/layers/packets/phy_packet.h"
#include "include/layers/packets/pres_packet.h"


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



void SAPLibPImpl::init( std::vector<std::string> intfs )
{
  std::unique_lock lk(_app_mu);
  _app_cv.wait(lk);

  // configure interfaces
  PhyPacket p( PhyPacket::set_intfs );
  
  std::ranges::for_each(intfs, [&](auto intf){
    const unsigned char len = (const uchar) intf.size();
    const unsigned char one = 1;
    p.append_ctrl_data((unsigned char) 1,  &len );
    p.append_ctrl_data((unsigned char) 1,  &one );
    p.append_data( intf.size(), (const unsigned char *) intf.c_str() );
  });

  //null character
  const unsigned char ZERO[1] = {0};
  p.append_data(1, ZERO );

  _pipeline.push( p.get_base() );

   
}


void SAPLibPImpl::finalize()
{

  auto input = PipelineInput(true);

  input.set_pkt_operation( common_layer_cmds::cleanup );

  _pipeline.push( std::move(input) );
 
}


app_error_t SAPLibPImpl::get_available_devices( app_intf::devices& di )
{
  PresentationPacket pp( PresentationPacket::get_devices );

  _pipeline.push( pp.get_base() );

  auto data = _pipeline.pop( PresentationPacket::get_devices );

  auto _di = app_intf::devices::deserialize( std::move(data.value()) ); //uses BasePacket;

  di = _di; 
  
  return app_error_t{};
}

SAPLibPImpl& GetDefaultSAPPImpl()
{
  std::cout << "GetDefaultSAPPImpl ..." << std::endl;
  static SAPLibPImpl lib_impl;
  return lib_impl;
}

