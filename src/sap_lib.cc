#include "include/sap_lib.h"
#include <iostream>
#include <vector>


SAPLib::SAPLib()
{
  std::cout << "SAPLib ctor" << std::endl;
}


void SAPLib::init(std::vector<std::string> intfs )
{
  std::cout << "Inside SAPLib::init()..." << std::endl;
  
  _impl.init(intfs);

}

std::map<uint, std::string> 
SAPLib::get_available_devices( )
{
  std::map<uint, std::string> out;
  app_intf::devices devs;

  _impl.get_available_devices( devs );

  out = devs.client_intf();
   
  return out;
}



void SAPLib::finalize()
{
  std::cout << "Inside SAPLib::finalize()..." << std::endl;
  
  _impl.finalize();

}
