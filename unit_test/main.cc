#include <iostream>
#include "include/sap.h"

int main(int argc, char * argv[]  )
{
  std::cout << "Hello World" << std::endl;
  SAP_Init(argc, argv);

  std::cin.ignore();

  SAP_Finalize();
  std::cout << "Application is complete..." <<  std::endl;

  return 0;
}
