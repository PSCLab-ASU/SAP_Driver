#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

struct NetworkPacket : public BasePacket
{
  using BasePacket::get_base;
   //restriction interface view
  NetworkPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}

};

template< typename InputType=NullType>
class  NetworkLayer : public base_layer<NetworkLayer<InputType>, NetworkPacket >
{

  public :
    using InputType_t   = InputType;
    using NetworkPktVec = std::vector<NetworkPacket>;

    NetworkLayer()
    {
      using namespace common_layer_cmds;
      using class_type = NetworkLayer<InputType>;

      const auto FromApp = Pipelineflow::FromApp;
      const auto FromPhy = Pipelineflow::FromPhy; 

      std::cout << "0)Registrating NetworkLayer functions" << std::endl;
      this->template register_cmd<FromApp>(self, &class_type::func0);
      std::cout << "1)Registrating NetworkLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command1, &class_type::func1);
      std::cout << "2)Registrating NetworkLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command2, &class_type::func2);
      std::cout << "3)Registrating NetworkLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command3, &class_type::func3);
      std::cout << "4)Registrating NetworkLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command4, &class_type::func4);
      std::cout << "5)Registrating NetworkLayer functions" << std::endl;
    }

    template<std::ranges::input_range R>
    NetworkLayer(R& r) 
    : NetworkLayer()
    {
      std::cout << "NetworkLayer ctor()" << std::endl;
    }

    template<std::ranges::input_range R>
    NetworkLayer(R& r, InputType& in) 
    : NetworkLayer()
    {
      std::cout << "NetworkLayer ctor(in)" << std::endl;
    }

    int func0(NetworkPacket&& in, NetworkPktVec& out )
    {
      std::cout << "Calling Network func0..." << std::endl;
      return 0;
    }

    int func1(NetworkPacket&& in, NetworkPktVec& out )
    {
      std::cout << "Calling Network func1..." << std::endl;
      return 0;
    }

    int func2(NetworkPacket&& in, NetworkPktVec& out )
    {

      std::cout << "Calling Network func2..." << std::endl;
      return 0;
    }

    int func3(NetworkPacket&& in, NetworkPktVec& out )
    {

      std::cout << "Calling Network func3..." << std::endl;
      return 0;
    }

    int func4(NetworkPacket&& in, NetworkPktVec& out )
    {

      std::cout << "Calling Network func4..." << std::endl;
      return 0;
    }
};

