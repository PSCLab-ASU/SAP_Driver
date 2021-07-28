#include <iostream>
#include "include/utils.h"
#include "include/app_structs.h"

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////Device Information ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
app_intf::devices
app_intf::devices::deserialize( PipelineOutput&& po )
{
  app_intf::devices devs;
  auto bp = BasePacket( po.get_data() );
  
  auto[dev_n_sz,  n_dev,  dev_n_ptr ] = bp.get_tlv(0); //src_mac
  auto n_devices = (unsigned char) *dev_n_ptr;

  for(int i=1; i < n_devices; i=i+2)
  {
    auto[id_sz,   n_id,   id_ptr ]   = bp.get_tlv(i); //src_mac
    auto[desc_sz, n_desc, desc_ptr ] = bp.get_tlv(i+1); //src_mac

    auto id_str = std::string((char *) id_ptr, id_sz);
    auto id = std::atoi( id_str.c_str() );
    auto desc = base_device_information::deserialize(desc_ptr);
    devs.add_device( id, desc );
  
  }

  return devs;
}

BasePacket app_intf::devices::serialize()
{
  BasePacket bp;
  unsigned char n_devices = _devices.size();

  bp.append_ctrl_data( (unsigned char) 1 ); 
  bp.append_ctrl_data((unsigned char) 1 );
  bp.append_data( 1, (const unsigned char *) &n_devices );

  for( auto[id, bdi] : _devices )
  {
    auto id_str = std::to_string(id);
    auto desc = bdi.serialize_desc();
 
    bp.append_ctrl_data( (unsigned char) id_str.size() );
    bp.append_ctrl_data( (unsigned char) 1 );
    bp.append_data( id_str.size(), (const unsigned char *) id_str.c_str() );

    bp.append_ctrl_data( (unsigned char) desc.size() ); 
    bp.append_ctrl_data((unsigned char) 1 );
    bp.append_data( desc.size(), (const unsigned char *) desc.data() );

  } 

  return bp;
}

std::map<uint, std::string > app_intf::devices::client_intf()
{
  std::map<uint, std::string> out;
  
  for( auto[dev_id, bdi] : _devices)
    out.emplace( dev_id, bdi.stringify_desc() );

  return out;

}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
