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

void SAPLib::finalize()
{
  std::cout << "Inside SAPLib::finalize()..." << std::endl;
  
  _impl.finalize();

}
