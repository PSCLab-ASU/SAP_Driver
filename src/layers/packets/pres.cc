#include "include/layers/packets/pres_packet.h"
#include "include/layers/pres_layer.h"

PresentationPacket::PresentationPacket( typename BasePacket::type base ) 
: BasePacket( base ) 
{}

PresentationPacket::PresentationPacket( ushort op ) 
: BasePacket( std::make_shared<base_pipeline_data>() ) 
{
  set_op( op );
}
