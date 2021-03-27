#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once
struct PresentationPacket : public BasePacket
{
  using BasePacket::get_base;
   //restriction interface view
  PresentationPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}

};

template< typename InputType=NullType>
class PresentationLayer : public base_layer<PresentationLayer<InputType>, PresentationPacket >
{
  public: 

    using InputType_t = InputType;
    using PresentationPktVec = std::vector<PresentationPacket>;

    PresentationLayer()
    {
      using namespace common_layer_cmds;
      using class_type = PresentationLayer<InputType>;

      const auto FromApp = Pipelineflow::FromApp;
      const auto FromPhy = Pipelineflow::FromPhy; 

      std::cout << "0)Registrating PresentationLayer functions" << std::endl;
      this->template register_cmd<FromApp>(self, &class_type::func0);
      std::cout << "1)Registrating PrensetationLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command1, &class_type::func1);
      std::cout << "2)Registrating PrensetationLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command2, &class_type::func2);
      std::cout << "3)Registrating PrensetationLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command3, &class_type::func3);
      std::cout << "4)Registrating PrensetationLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command4, &class_type::func4);
      std::cout << "5)Registrating PrensetationLayer functions" << std::endl;

    }

    template<std::ranges::input_range R>
    PresentationLayer( R& r ) 
    : PresentationLayer()
    {
      std::cout << "PresentationLayer ctor " << std::endl;
    }

    template<std::ranges::input_range R>
    PresentationLayer( R& r, InputType& in ) 
    : PresentationLayer()
    {
      std::cout << "PresentationLayer ctor(in) " << std::endl;
    }

    template<std::ranges::input_range R>
    PresentationLayer( R& r, NullType in ) 
    : PresentationLayer()
    {
      std::cout << "Presentation Layer (NullType) " << std::endl;
    }

    int func0(PresentationPacket&& in, PresentationPktVec& out )
    {
      std::cout << "Calling Presentation func0..." << std::endl;
      return 0;
    }
      
    int func1(PresentationPacket&& in, PresentationPktVec& out )
    {
      std::cout << "Calling Presentation func1..." << std::endl;
      return 0;
    }

    int func2(PresentationPacket&& in, PresentationPktVec& out )
    {

      std::cout << "Calling Presentation func2..." << std::endl;
      return 0;
    }

    int func3(PresentationPacket&& in, PresentationPktVec& out )
    {

      std::cout << "Calling Presentation func3..." << std::endl;
      return 0;
    }

    int func4(PresentationPacket&& in, PresentationPktVec& out )
    {

      std::cout << "Calling Presentation func4..." << std::endl;
      return 0;
    }
};

