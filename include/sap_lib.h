#include "include/sap_impl.h"
#include <mutex>
#include <vector>
#include <map>

class SAPLib
{

  public :

    SAPLib();

    void init( std::vector<std::string> );

    std::map<uint, std::string> get_available_devices( );

    void finalize();

  private:


    static std::mutex _mu;
    SAPLibPImpl& _impl = GetDefaultSAPPImpl();


};
