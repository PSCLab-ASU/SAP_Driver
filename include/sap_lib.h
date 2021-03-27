#include "include/sap_impl.h"
#include <mutex>


class SAPLib
{

  public :

    SAPLib();

    void init();

    void finalize();

  private:


    static std::mutex _mu;
    SAPLibPImpl& _impl = GetDefaultSAPPImpl();


};
