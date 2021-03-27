#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"

#pragma once
struct TransportPacket : public BasePacket
{
  using BasePacket::get_base;
   //restriction interface view
  TransportPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}

};

template< typename InputType=NullType>
class TransportLayer : public base_layer<TransportLayer<InputType>, TransportPacket >
{

  public :

    using InputType_t     = InputType;
    using TransportPktVec = std::vector<TransportPacket>;

    TransportLayer()
    {
      using namespace common_layer_cmds;
      using class_type = TransportLayer<InputType>;

      const auto FromApp = Pipelineflow::FromApp;
      const auto FromPhy = Pipelineflow::FromPhy; 

      std::cout << "0)Registrating TransportLayer functions" << std::endl;
      this->template register_cmd<FromApp>(self, &class_type::func0);
      std::cout << "1)Registrating TransportLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command1, &class_type::func1);
      std::cout << "2)Registrating TransportLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command2, &class_type::func2);
      std::cout << "3)Registrating TransportLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command3, &class_type::func3);
      std::cout << "4)Registrating TransportLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command4, &class_type::func4);
      std::cout << "5)Registrating TransportLayer functions" << std::endl;

    }
    

    template<std::ranges::input_range R>
    TransportLayer(R& r)
    : TransportLayer()
    {
      std::cout << "TransportLayer ctor()" << std::endl;
    }

    template<std::ranges::input_range R>
    TransportLayer(R& r, InputType& in)
    : TransportLayer()
    {
      std::cout << "TransportLayer ctor(in)" << std::endl;
    }

    int func0(TransportPacket&& in, TransportPktVec& out )
    {
      std::cout << "Calling Transport func0..." << std::endl;
      return 0;
    }

    int func1(TransportPacket&& in, TransportPktVec& out )
    {
      std::cout << "Calling Transport func1..." << std::endl;
      return 0;
    }

    int func2(TransportPacket&& in, TransportPktVec& out )
    {

      std::cout << "Calling Transport func2..." << std::endl;
      return 0;
    }

    int func3(TransportPacket&& in, TransportPktVec& out )
    {

      std::cout << "Calling Transport func3..." << std::endl;
      return 0;
    }

    int func4(TransportPacket&& in, TransportPktVec& out )
    {

      std::cout << "Calling Transport func4..." << std::endl;
      return 0;
    }
};

