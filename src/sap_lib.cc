#include "include/sap_lib.h"
#include <iostream>

SAPLib::SAPLib()
{
  std::cout << "SAPLib ctor" << std::endl;
}


void SAPLib::init()
{
  std::cout << "Inside SAPLib::init()..." << std::endl;
  
  _impl.init();

}

void SAPLib::finalize()
{
  std::cout << "Inside SAPLib::finalize()..." << std::endl;
  
  _impl.finalize();

}
