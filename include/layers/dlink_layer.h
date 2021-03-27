#include <ranges>
#include <iostream>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once
struct DatalinkPacket : public BasePacket
{
  using BasePacket::get_base;
   //restriction interface view
  DatalinkPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}

};

template< typename InputType=NullType>
class DatalinkLayer : public base_layer<DatalinkLayer<InputType>, DatalinkPacket >
{
  public :
    using InputType_t    = InputType;
    using DatalinkPktVec = std::vector<DatalinkPacket>;

    DatalinkLayer()
    {
      using namespace common_layer_cmds;
      using class_type = DatalinkLayer<InputType>;

      const auto FromApp = Pipelineflow::FromApp;
      const auto FromPhy = Pipelineflow::FromPhy; 

      std::cout << "0)Registrating DatalinkLayer functions" << std::endl;
      this->template register_cmd<FromApp>(self, &class_type::func0);
      std::cout << "1)Registrating DatalinkLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command1, &class_type::func1);
      std::cout << "2)Registrating DatalinkLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command2, &class_type::func2);
      std::cout << "3)Registrating DatalinkLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command3, &class_type::func3);
      std::cout << "4)Registrating DatalinkLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command4, &class_type::func4);
      std::cout << "5)Registrating DatalinkLayer functions" << std::endl;
    }

    template<std::ranges::input_range R>
    DatalinkLayer(R& r) 
    : DatalinkLayer()
    {
      std::cout << "DataLinkLayer ctor()" << std::endl;
    }

    template<std::ranges::input_range R>
    DatalinkLayer(R& r, InputType& in) 
    : DatalinkLayer()
    {
      std::cout << "DataLinkLayer ctor(in)" << std::endl;
    }

    int func0(DatalinkPacket&& in, DatalinkPktVec& out )
    {
      std::cout << "Calling Datalink func0..." << std::endl;
      return 0;
    }
  
    int func1(DatalinkPacket&& in, DatalinkPktVec& out )
    {
      std::cout << "Calling Datalink func1..." << std::endl;
      return 0;
    }

    int func2(DatalinkPacket&& in, DatalinkPktVec& out )
    {

      std::cout << "Calling Datalink func2..." << std::endl;
      return 0;
    }

    int func3(DatalinkPacket&& in, DatalinkPktVec& out )
    {

      std::cout << "Calling Datalink func3..." << std::endl;
      return 0;
    }

    int func4(DatalinkPacket&& in, DatalinkPktVec& out )
    {

      std::cout << "Calling Datalink func4..." << std::endl;
      return 0;
    }
};

