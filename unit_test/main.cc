#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
#include "include/sap.h"

int main(int argc, char * argv[]  )
{
  std::cout << "Hello World" << std::endl;
  uint n_devices =0, new_ndevices =0;
  SAP_Init(argc, argv);

  std::cin.ignore();

  //Get number of accelerators
  SAP_GetNDevices( &n_devices );

  std::cin.ignore();

  printf("UNIT_TEST: Found %i devices\n", n_devices);
  
  std::vector<uint> dev_ids(n_devices, 0);

  //Get all device IDs
  SAP_GetDeviceIDs( n_devices, dev_ids.data(), &new_ndevices );

  printf("UNIT_TEST: DEV_IDs = { " );

  for(int i=0; i < dev_ids.size() - 1; i++)
    printf("%i, ", dev_ids[i] );
  printf("%i } \n", dev_ids.back() );
  
/*
  //Get all the descriptions
  std::ranges::for_each( dev_ids, []( uint d_id ) 
  {
    std::string dev = "Device ID = " + std::to_string(d_id) + " (";

    for( int i=0; i < SW_VERSION_ID+1; i++){
       uint val =0;
       SAP_GetDeviceDesc( d_id, (DEV_DESC_ATTR)i, &val);
       dev += std::to_string(val) + ":";
    }
 
    printf("%s)\n", dev.c_str() );

  });

  std::cin.ignore();
  */
  SAP_Finalize();
  std::cout << "Application is complete..." <<  std::endl;

  return 0;
}
