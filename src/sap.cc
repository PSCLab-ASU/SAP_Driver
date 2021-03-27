#include "include/sap.h"
#include "include/sap_lib.h"
#include "include/utils.h"
#include <iostream>

void SAP_Init()
{
  std::cout << "Inside SAP_init()..." << std::endl;
  SAPLib _sap; //getting 

  _sap.init();

}

void SAP_Finalize()
{
  std::cout << "Inside SAP_init()..." << std::endl;
  SAPLib _sap; //getting 

  _sap.finalize();

}
