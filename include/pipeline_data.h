#include <tuple>
#include <vector>
#include <include/utils.h>
#include <type_traits>
#include <iostream>
#include <memory>

#pragma once

#define CTRL_SZ 1
#define DATA_SZ 0 

enum Pipelineflow { FromApp=0, FromPhy };

struct base_pipeline_data
{

  base_pipeline_data(size_t ctrl_size=CTRL_SZ, size_t data_size=DATA_SZ)
  : _ctrl(ctrl_size, 0), _data(data_size, 0), dst(false)
  {
    std::cout << "base_pipeline_data(....) " << std::endl; 
  }

  void mark_as_response()
  {
    std::cout << "Mark as response... " << dst << std::endl;  
    dst = true;
  }

  void reset_dst()
  {
    dst = false;
  }

  const bool& get_dst()
  {
    //std::cout << "DST : " << dst << std::endl;  
    return dst;
  }

  void set_op(uchar op)
  {
    std::cout << "set_op() : " << std::to_string(op) << " : " << _ctrl.size() <<  std::endl;
    _ctrl[0] = op;
  }

  uchar get_op(){
    std::cout << "get_op()" << std::endl;
    return _ctrl[0];
  }

  auto get_ctrl()
  {
    return _ctrl;
  }

  void set_ctrl( const std::vector<unsigned char>& ctrl )
  {
    _ctrl = ctrl;
  }
 
  void pack_header( const std::vector<uchar>& hdr )
  {
    //TBD
  }

  // 0 = downstream, 1 = upstream
  bool dst = false;
  //this vector manages the control signals
  //and is reset every current layer  
  std::vector<unsigned char> _ctrl;

  //unaffected payload data
  std::vector<unsigned char> _data;

};

struct BasePacket
{
  using type = std::shared_ptr<base_pipeline_data>;

  BasePacket(){}

  BasePacket( type base )
  : _base (base)
  {}

  type& get_base()
  { 
    return _base;
  }

  auto get_ctrl()
  {
    return _base->get_ctrl();
  }
 
  template<typename Ctrl>
  void update_ctrl( Ctrl&& ctl )
  {
    _base->set_ctrl( ctl.get() );
  }

  template<typename T>
  void pack_header( std::vector<T> header )
  {
    //_base->pack_header( hdr );
  }  

  void mark_as_resp()
  {
    _base->mark_as_response(); 
  }

  void reset_dst()
  {
    _base->reset_dst();
  }

  private:

    type _base;
};

template<Pipelineflow plf>
struct pipeline_data 
{

  pipeline_data(bool alloc = false )
  : _heartbeat(0)
  {
    std::cout << "pipeline_data ctor()" << std::endl;
    if( alloc )
    {
      std::cout << "Allocating base packet ..." << std::endl;
      _data = std::make_shared<base_pipeline_data>();
    }
  }

  pipeline_data( int hb )
  : _heartbeat(hb)
  {
    std::cout << "pipeline_data ctor("<< hb << ")" << std::endl;
  }
 
  pipeline_data(std::shared_ptr<base_pipeline_data> data )
  : _heartbeat(0), _data( data )
  {
    std::cout << "pipeline_data ctor("<< _heartbeat << ", data)" << std::endl;
  }

  explicit operator auto()
  {
    if constexpr ( plf == Pipelineflow::FromApp )
    {
      //converting from Downstream to Upstream
      auto out = pipeline_data<Pipelineflow::FromPhy>( _data );
      return out;
    }
    else
    {
      //converting Upstream to Downstream
      auto out = pipeline_data<Pipelineflow::FromApp>( _data );
      return out;
    }

  }

  operator int(){ return _heartbeat; }
 
  auto get_data(){ return _data; }

  operator bool(){ return (bool)_data; } 

  void set_pkt_operation(uchar op){
    if( (bool) _data )
      _data->set_op( op );
  }

  uchar get_pkt_operation(){
    if( (bool) _data )
      return _data->get_op();
    else
      return 0; //<----- 0 will always be self
  } 

  const bool& get_dst() const
  {
    if( (bool) _data )
      return _data->get_dst();
    else
      return queue_dst;
  }

  void reset_dst()
  {
    if( (bool) _data )
      return _data->reset_dst();
  }

  private:

    int _heartbeat;
    bool queue_dst = false; //down stream = 0, internal queue = 1
    std::shared_ptr<base_pipeline_data> _data;
};


typedef pipeline_data<Pipelineflow::FromApp> PipelineInput;
typedef pipeline_data<Pipelineflow::FromPhy> PipelineOutput;

template<Pipelineflow plf>
bool operator==(const pipeline_data<plf>& lhs, const pipeline_data<plf>& rhs)
{
  return true;
}

template<Pipelineflow plf>
bool operator!=(const pipeline_data<plf>& lhs, const pipeline_data<plf>& rhs)
{
  return !(lhs == rhs);
}


