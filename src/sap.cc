#include <iostream>
#include <vector>
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
