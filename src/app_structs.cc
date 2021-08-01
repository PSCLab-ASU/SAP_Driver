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
  
  printf("1) app_intf::devices::deserialize(...)\n");

  auto[n_dev, dev_n_sz, dev_n_ptr ] = bp.get_tlv(0); //src_mac
  auto n_devices = (unsigned char) *dev_n_ptr;

  printf("2) app_intf::devices::deserialize(...) : total dev = %i\n", n_devices);
  

  for(int i=0; i < n_devices; i++)
  {
    auto[n_id, id_sz,  id_ptr ]   = bp.get_tlv(2*i+1); //src_mac
    auto[n_desc, desc_sz, desc_ptr ] = bp.get_tlv(2*(i+1) ); //src_mac

    auto id_str = std::string((char *) id_ptr, id_sz);
    printf("3) app_intf::devices::deserialize(...) id = '%02x:%02x', size %lu \n", id_str[0], id_str[1], id_str.size() );
    auto id = std::atoi( id_str.c_str() );
    auto desc = base_device_information::deserialize(desc_ptr);
    devs.add_device( id, desc );
  
  }

  printf("ret) app_intf::devices::deserialize(...)\n");
  return devs;
}

BasePacket app_intf::devices::serialize()
{
  BasePacket bp;
  unsigned char n_devices = _devices.size();

  printf("1) app_intf::devices::serialize(...) : total_devs : %i\n", n_devices);
  bp.append_ctrl_data( (unsigned char) 1 ); 
  bp.append_ctrl_data((unsigned char) 1 );
  bp.append_data( 1, (const unsigned char *) &n_devices );
  printf("2) app_intf::devices::serialize(...)\n");

  for( auto [id, bdi] : _devices )
  {
    printf("app_intf::devices::serialize(...) %i \n", id);
    auto desc = bdi.serialize_desc();

    std::string id_str = std::to_string( id ); 
    bp.append_ctrl_data( (unsigned char) 1 );
    bp.append_ctrl_data( (unsigned char) id_str.size() );
    bp.append_data( id_str.size() , (const unsigned char *) id_str.c_str() );

    bp.append_ctrl_data((unsigned char) 1 );
    bp.append_ctrl_data( (unsigned char) desc.size() ); 
    bp.append_data( desc.size(), (const unsigned char *) desc.data() );
    
  } 

  printf("ret) app_intf::devices::serialize(...)\n");
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
