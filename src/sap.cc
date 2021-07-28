#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "include/sap.h"
#include "include/sap_lib.h"
#include "include/utils.h"

void SAP_Init(int argc, char * argv[] )
{
  std::cout << "Inside SAP_init()..." << std::endl;
  SAPLib _sap; //getting 
  std::vector<std::string> intf_names;

  //check commandline parameter
  for(int i = 0; i < argc; i++ )
    if( std::string( argv[i] ) ==  std::string("-sap_intfs") )
       boost::split(intf_names, std::string(argv[i+1]), boost::is_any_of(",") );   


  _sap.init(intf_names);

}

void SAP_Finalize()
{
  std::cout << "Inside SAP_init()..." << std::endl;
  SAPLib _sap; //getting 

  _sap.finalize();

}


void SAP_GetNDevices( uint * ndevices)
{
  std::cout << "Inside SAP_GetNDevices..." << std::endl;
  SAPLib _sap; //getting 

  auto devs = _sap.get_available_devices();
  *ndevices = devs.size();
}

void SAP_GetDeviceIDs( uint ndevices, uint devices[], uint* nndevices )
{
  std::cout << "Inside SAP_GetDeviceIDs..." << std::endl;
  SAPLib _sap; //getting 

  auto devs = _sap.get_available_devices();
  uint _ndevices = devs.size();  

  auto It = devs.begin();
  for(int i=0; i < std::min(ndevices, _ndevices ); i++, It++)
    devices[i] = It->first;

  *nndevices = devs.size(); 

}

void SAP_GetDeviceDesc( uint dev_id, enum DEV_DESC_ATTR desc_idx, uint * val)
{
  std::cout << "Inside SAP_GetDeviceDesc..." << std::endl;
  std::vector<std::string> _desc;
  SAPLib _sap; //getting 

  auto devs = _sap.get_available_devices();

  boost::split(_desc, devs.at(dev_id), boost::is_any_of(":") );   
  
  *val = std::atoi(_desc[desc_idx].c_str() );
}
