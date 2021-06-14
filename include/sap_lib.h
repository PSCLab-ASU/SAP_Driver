#include "include/sap_impl.h"
#include <mutex>
#include <vector>

class SAPLib
{

  public :

    SAPLib();

    void init( std::vector<std::string> );

    void finalize();

  private:


    static std::mutex _mu;
    SAPLibPImpl& _impl = GetDefaultSAPPImpl();


};
