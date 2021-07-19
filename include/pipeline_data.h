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

  void set_src(std::string src_mac)
  {
    _src = src_mac;
  }

  auto get_src() const
  {
    return _src;
  }

  uchar& get_op(){
    std::cout << "get_op()" << std::endl;
    return _ctrl[0];
  }

  decltype(auto) get_ctrl()
  {
    return (_ctrl);
  }

  auto get_ctrl_with_offset() const
  {
    //skipping the op code
    auto ctrl_ptr = &_ctrl.at(_ctrl_offset+1);
    return ctrl_ptr; 
  }

  auto get_ctrl_with_offset()
  {
    //skipping the op code
    auto ctrl_ptr = &_ctrl.at(_ctrl_offset+1);
    return ctrl_ptr; 
  }

  decltype(auto) get_data()
  {
    return (_data);
  }

  auto get_extra()
  {
    return _extra_data;
  }

  auto set_extra( std::shared_ptr<base_pipeline_data> extra_data )
  {
    _extra_data = extra_data;
  }

  auto get_data_with_offset() const
  {
    printf("get_data_with_offset() const : %zu, size : %zu\n", _data_offset, _data.size() );
    auto data_ptr = &_data.at(_data_offset);
    return data_ptr;
  }

  auto get_data_with_offset()
  {
    printf("get_data_with_offset() : %zu, size : %zu\n", _data_offset, _data.size() );
    auto data_ptr = &_data.at(_data_offset);
    return data_ptr;
  }

  void set_ctrl( const std::vector<unsigned char>& ctrl )
  {
    _ctrl = ctrl;
  }

  void append_ctrl( size_t nbytes, const uchar * data )
  {
    auto c_sz= _ctrl.size();
    if( c_sz == 0 ) { _ctrl.push_back(0); _ctrl.push_back(nbytes); }
    if( c_sz == 1 ) _ctrl.push_back(nbytes);
    else _ctrl.at(1) += nbytes; 
 
    for(size_t i=0; i < nbytes; i++) 
    {
      printf(" Appending ctrl data : %i size = %llu \n", data[i], _ctrl.size() );
      _ctrl.push_back( data[i] );
    }
  }

  void allocate_data( size_t bytes )
  {
    _data = std::vector<unsigned char>(bytes, 0);
  }

  void append_data( size_t nbytes, const uchar * data )
  {
    for(size_t i=0; i < nbytes; i++) 
      _data.push_back( data[i] );
  }
 
  void pack_header( const std::vector<uchar>& hdr )
  {
    //TBD
  }

  void set_intf_id( int id )
  {
    _intf_id = id;
  }

  int get_intf_id() const
  {
    return _intf_id;
  }

  void set_incr_offsets( std::optional<size_t> ctrl_offset, 
                         std::optional<size_t> data_offset={}, 
                         bool incr_offset = true )
  {
    if( ctrl_offset ){
      printf("set_incr_offset ctrl %zu\n", ctrl_offset.value() );
      if( incr_offset ) _ctrl_offset += ctrl_offset.value()+1;
      else _ctrl_offset = ctrl_offset.value()+1;
    }

    if ( data_offset ){
      printf("set_incr_offset data %zu\n", data_offset.value() );
      if( incr_offset ) _data_offset += data_offset.value()+1;
      else _data_offset = data_offset.value()+1;
      printf("end - set_incr_offset data %zu\n", _data_offset );
    }
  }

  uint get_max_tlv(bool recur=true) const
  {
    if( !recur && _complete ) return 0;
 
    auto ctrl_size = _ctrl.size() - (_ctrl_offset + 2);
    ctrl_size /= 2; //each pair is a tl combination
    //get predecssor
    if( recur && _extra_data ) ctrl_size += _extra_data->get_max_tlv();
    
    if(_ctrl.size() > 2 ) printf("PLD ctrl %i, %i, %i \n", _ctrl[0], _ctrl[1], _ctrl[2] );

    printf("PLD get_max_tlv  ctrl_size= %llu, mtlv = %i, recur = %i, extra = %i\n ", 
           _ctrl.size(), ctrl_size, recur, (bool) _extra_data);

    return (ctrl_size);
  }

  std::tuple<size_t, size_t, const uchar * >
  get_tlv( uint idx ) const
  {
    auto max_tlv = get_max_tlv(false);
    auto max_recur_tlv = get_max_tlv(true);
    if( idx < max_tlv){
      size_t byte_offset = 0;
      auto ctrl = get_ctrl_with_offset();
      size_t v_len = ctrl[2*idx]; 
      size_t e_sz  = ctrl[2*idx + 1];
  
      for(int i=0; i < idx; i++) byte_offset += ctrl[2*i] * ctrl[2*i+1];
  
      const uchar * ptr = get_data_with_offset() + byte_offset;

      //auto out = std::make_tuple<size_t, size_t, const uchar *>
      auto out = std::make_tuple( e_sz, v_len, ptr ); 
      return out;
    }
    else if( idx < max_recur_tlv){
      return _extra_data->get_tlv(idx - max_tlv );
    }
    else{
      throw std::out_of_range("DL: TLV is out of bounds idx =" + 
                               std::to_string(idx) + ", max_tlv = " + 
                               std::to_string(max_tlv) + ", max_recur_tlv = " +
                               std::to_string(max_recur_tlv) + "\n" );
    }
                      
  }
 
  bool is_complete() { return _complete; }
  void complete() { _complete =true; }

  void advance_tlv(uint idx )
  {
    auto max_tlv = get_max_tlv(false);
    if( idx < max_tlv){
      auto ctrl = get_ctrl_with_offset();
      //calc the total number of bytes to incr
      //the data
      size_t nbytes=0;
      for(int i=0; i < idx; i++) 
        nbytes += ctrl[2*i] * ctrl[2*i+1];

      set_incr_offsets( 2*idx, nbytes);
      
    }
    else if( idx > max_tlv){
      _extra_data->advance_tlv( idx - max_tlv );
      if(!is_complete() ) complete(); 
    }
    else{
      if(!is_complete() ) complete(); 
    }
    
  }

  //is complete forces a 0 remaining TLV
  bool _complete=false;

  //setting src address
  std::string _src;

  //which interface it came from
  int _intf_id;

  // 0 = downstream, 1 = upstream
  bool dst = false;
  //this vector manages the control signals
  //and is reset every current layer  
  size_t _ctrl_offset = 0;
  std::vector<unsigned char> _ctrl;

  //unaffected payload data
  size_t _data_offset = 0;
  std::vector<unsigned char> _data;

  std::shared_ptr<base_pipeline_data> _extra_data;
};

struct BasePacket
{
  using type = std::shared_ptr<base_pipeline_data>;

  BasePacket(){}

  BasePacket( type base )
  : _base (base)
  { }

  type get_base()
  { 
    return _base;
  }

  template<bool offset = true>
  decltype(auto) get_data()
  {
    if constexpr (offset) 
      return _base->get_data_with_offset();
    else
      return (_base->get_data() );
  }

  template<bool offset = true>
  decltype(auto) get_ctrl() const
  {
    std::cout << "base ctrl pointer : " << ((bool) _base) <<  std::endl;
    if constexpr( offset )
      return _base->get_ctrl_with_offset();
    else
      return (_base->get_ctrl());
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
  
  void append_ctrl_data( size_t nbytes, const unsigned char * buff)
  {
    _base->append_ctrl(nbytes, buff);
  }

  template<typename T>
  void append_ctrl_data( T ctrl_val)
  {
    //_base->append_ctrl(sizeof(T), (const unsigned char *) &ctrl_val);
    _base->append_ctrl(1, (const unsigned char *) &ctrl_val);
  }

  void append_data( size_t nbytes, const unsigned char * buff)
  {
    _base->append_data(nbytes, buff);
  }

  template<typename T>
  void append_data( T data_val)
  {
    _base->append_data(sizeof(T), (const unsigned char *) &data_val );
  }

  void allocate_data(size_t bytes)
  {
    _base->allocate_data(bytes);
  }

  template<typename T>
  void append_data( std::vector<T> in )
  {
    size_t nbytes = sizeof(T)*in.size();
    void * raw = in.data();
       
    _base->append_data( nbytes, (uchar *) raw );
  }

  void set_op( ushort op )
  {
    _base->set_op( op );
  }

  ushort get_op( )
  {
    return _base->get_op( );
  }

  void set_src( std::string src )
  {
    _base->set_src( src );
  }

  std::string get_src( ) const
  {
    return _base->get_src( );
  }

  void mark_as_resp()
  {
    _base->mark_as_response(); 
  }

  void reset_dst()
  {
    _base->reset_dst();
  }

  void set_intf_id( int id )
  {
    _base->set_intf_id(id);
  }

  int get_intf_id() const
  {
    return _base->get_intf_id();
  }

  void set_offsets( size_t ctrl_offset, size_t data_offset= 0,
                    bool incr_offset = true )
  {
    std::optional<size_t>  c_offset, d_offset;

    if( ctrl_offset ) c_offset = ctrl_offset;
    if( data_offset ) d_offset = data_offset;
 
    _base->set_incr_offsets( c_offset, d_offset, incr_offset);
  }
  
  std::tuple<size_t, size_t, const uchar * >
  get_tlv( uint idx ) const
  {
    //this number will include all the recursion
    auto max_tlvs = _base->get_max_tlv();
    if( idx > max_tlvs ) 
    {
      
      throw std::out_of_range("DL: get_tlv TLV is out of bounds : idx=" + 
                               std::to_string(idx) );
    }
    //run through control and calculate total offset bytes
    //recurrent operation
    auto tlv_tup = _base->get_tlv( idx );

    return tlv_tup;
  }

  void fuse_extra( type extra_base )
  {
    _base->set_extra( extra_base );
  }

  void advance_tlv( uint idx)
  {
    //Need to update the offsets
    std::optional<size_t>  c_offset, d_offset;
    auto max_tlvs = _base->get_max_tlv();

    //run through control and calculate total offset bytes
    //recurrent operation
    if( idx < max_tlvs ) {
      _base->advance_tlv(idx);
    }
    else{ 
      throw std::out_of_range("DL: TLV is out of bounds : idx = " + 
                               std::to_string(idx) );
    }

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

  const uchar& get_pkt_operation(){
    if( (bool) _data )
    {
      return _data->get_op();
    }
    else
      return empty; //<----- 0 will always be self
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

    uchar empty = 0;
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


