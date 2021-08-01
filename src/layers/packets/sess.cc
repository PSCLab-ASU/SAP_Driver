#include "include/layers/packets/sess_packet.h"
#include "include/layers/sess_layer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Session Packet Interface......//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
SessionPacket::SessionPacket(typename BasePacket::type base )
: BasePacket (base)
{


}

SessionPacket::SessionPacket( ushort op )
: BasePacket( std::make_shared<base_pipeline_data>() )
{
  set_op( op );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Session Packet device info......////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

SessionPacket::device_information 
SessionPacket::device_information::deserialize( const SessionPacket& in )
{
  
  accel_descriptor acd;
  auto[n_descs,    desc_sz,    desc_ptr]     = in.get_tlv(0); //Accel desc
  auto[n_doccs,    docc_sz,    docc_ptr]     = in.get_tlv(2); //device occupancy
  auto[n_dreprogs, dreprog_sz, dreprog_ptr] = in.get_tlv(3); //device reprog
  auto[n_dev_ids,  dev_id_sz,  dev_id_ptr]   = in.get_tlv(6); //device id
  auto[n_alcongs,  alcong_sz,  alcong_ptr]   = in.get_tlv(7); //Avg link congest

  /*printf("SESS DESC: %i, %i, \n",      desc_sz,    n_descs);
  printf("SESS DEVOCC: %i, %i, \n",    docc_sz,    n_doccs);
  printf("SESS DEVREPROG: %i, %i, \n", dreprog_sz, n_dreprogs);
  printf("SESS DEV_ID: %i, %i, \n",    dev_id_sz,  n_dev_ids);
  printf("SESS ALCONG: %i, %i, \n",    alcong_sz,  n_alcongs);
  */

  for(int i=HW_VID; i < ACCEL_ID_END; i++)
  {
    unsigned int desc = ((ushort *) desc_ptr)[i];
    acd[(accel_desc_param)i] = desc;
  }

  ushort link_cong  = ( (ushort *) alcong_ptr)[0];
  uchar d_occupancy = docc_ptr[0];
  uchar d_reprog    = dreprog_ptr[0];
  uchar d_id        = dev_id_ptr[0];
   
  auto dev = device_information( std::to_string(d_id),  link_cong, d_occupancy, d_reprog);

  dev.set_desc( acd );

  return dev;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Shared memory methods.....//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool SessSM::device_exists( std::string dev_id )
{
  std::lock_guard lk(_devices.first); 

  return
  std::ranges::any_of( _devices.second, equal_to{dev_id}, 
                      &SessionPacket::device_information::get_id );
}

SessionPacket::device_information& 
SessSM::get_device_info( std::string dev_id )
{  
  std::lock_guard lk(_devices.first);

  return 
  *std::ranges::find_if( _devices.second, equal_to{dev_id}, 
                         &SessionPacket::device_information::get_id );
}

std::map<std::string, std::string >
SessSM::serialize_device_info()
{
  std::map<std::string ,std::string> id_descs;

  std::lock_guard lk(_devices.first);  

  std::ranges::for_each(_devices.second,  [&](auto dev)
  {
     if( dev.is_active() )
     {
       auto id = dev.get_id();
       auto desc = dev.serialize_desc();
       auto desc_str = std::string((char *)desc.data(), desc.size() );
       id_descs[id] = desc_str;
     }

  } ); 

  return id_descs;

}
