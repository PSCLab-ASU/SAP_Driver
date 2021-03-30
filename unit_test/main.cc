#include <iostream>
#include "include/sap.h"

int main(int argc, char * argv[]  )
{
  std::cout << "Hello World" << std::endl;
  SAP_Init();

  std::cout << "Finalizing..." <<  std::endl;
  std::cin.get();
  SAP_Finalize();

  std::cout << "Hit Enter to exit..." <<  std::endl;
  std::cin.get();

  return 0;
}
