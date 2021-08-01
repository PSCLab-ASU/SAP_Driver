#include <iostream>
#include "include/utils.h"
#include "include/pipeline_data.h"


struct app_error_t {};


namespace app_intf
{

  struct devices
  {

    static devices deserialize( PipelineOutput&& );
 
    std::map<uint, std::string> client_intf();

    void add_device( uint id, const base_device_information& bdi)
    {
      printf("app_intf::devices::add_device(...) : id = %i \n", id);
      _devices[id] = bdi;
    }

    BasePacket serialize();

    std::map<uint, base_device_information> _devices;

  };

}
