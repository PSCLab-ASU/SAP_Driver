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

std::optional<unsigned char * >
SessionPacket::try_extract_dev_info()
{

  return {};
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Session Packet device info......////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
SessionPacket::device_information& 
SessionPacket::device_information::operator =(const SessionPacket::device_information& rhs)
{

  return *this;
}

SessionPacket::device_information 
SessionPacket::device_information::deserialize( unsigned char *)
{
  return device_information{};
}

bool operator==( const SessionPacket::device_information& lhs,
                 const SessionPacket::device_information& rhs )
{
  return ( lhs.get_id() == rhs.get_id() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Shared memory methods.....//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool SessSM::device_exists( const SessionPacket::device_information& cand_dev) const
{
  return true;
}

bool SessSM::exact_match(const SessionPacket::device_information& cand_dev ) const
{
  return true;
}

SessionPacket::device_information& 
SessSM::get_device_info( const SessionPacket::device_information& dev_info )
{
  return _devices.second[0]; 
  
}

