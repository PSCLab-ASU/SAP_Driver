#include "include/layers/trans_layer.h"
#include "include/layers/packets/trans_packet.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Transport Packet Interface......////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
TransportPacket::TransportPacket(typename BasePacket::type base )
: BasePacket (base)
{


}

TransportPacket::TransportPacket( ushort op )
: BasePacket( std::make_shared<base_pipeline_data>() )
{
  set_op( op );
}

std::optional<unsigned char * >
TransportPacket::try_extract_dev_info()
{

  return {};
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Transport Packet device info......////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
TransportPacket::device_information::device_information()
{

}

TransportPacket::device_information::device_information( const TransportPacket::device_information&)
{

}

TransportPacket::device_information& 
TransportPacket::device_information::operator =(const TransportPacket::device_information& rhs)
{

  return *this;
}

TransportPacket::device_information 
TransportPacket::device_information::deserialize( unsigned char *)
{
  return device_information{};
}

bool operator==( const TransportPacket::device_information& lhs,
                 const TransportPacket::device_information& rhs )
{
  return ( lhs.get_id() == rhs.get_id() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////.....Shared memory methods.....//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool TransSM::device_exists( const TransportPacket::device_information& cand_dev) const
{

  return true;
}

bool TransSM::exact_match(const TransportPacket::device_information& cand_dev ) const
{

  return true;
}

TransportPacket::device_information& 
TransSM::get_device_info( const TransportPacket::device_information& dev_info )
{


  return  _devices.second[0];

}

