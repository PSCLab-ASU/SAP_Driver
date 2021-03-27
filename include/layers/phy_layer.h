#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

struct PhyPacket : public BasePacket
{
  using BasePacket::get_base;
   //restriction interface view
  PhyPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}

};

template< typename InputType=NullType>
class PhyLayer : public base_layer<PhyLayer<InputType>, PhyPacket >
{
  
  public :
    using InputType_t = InputType;
    using PhyPktVec   = std::vector<PhyPacket>;

    PhyLayer()
    {
      
      using namespace common_layer_cmds;
      using class_type = PhyLayer<InputType>;

      const auto FromApp = Pipelineflow::FromApp;
      const auto FromPhy = Pipelineflow::FromPhy; 

      std::cout << "0)Registrating PhyLayer functions" << std::endl;
      this->template register_cmd<FromApp>(self, &class_type::func0);
      std::cout << "1)Registrating PhyLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command1, &class_type::func1);
      std::cout << "2)Registrating PhyLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command2, &class_type::func2);
      std::cout << "3)Registrating PhyLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command3, &class_type::func3);
      std::cout << "4)Registrating PhyLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command4, &class_type::func4);
      std::cout << "5)Registrating PhyLayer functions" << std::endl;

    }
 
    template<std::ranges::input_range R>
    PhyLayer(R& r)
    : PhyLayer()
    {
        
      std::cout << "PhyLayer ctor() : " << std::is_same_v<NullType, InputType> << std::endl;
    }

    template<std::ranges::input_range R>
    PhyLayer(R& r, InputType& in )
    : PhyLayer()
    {
      std::cout << "PhyLayer ctor(in)" << std::endl;
    }

    int func0(PhyPacket&& in, PhyPktVec& out )
    {

      std::cout << "Calling PHY func0..." << std::endl;
      return 0;
    }

    int func1(PhyPacket&& in, PhyPktVec& out )
    {
      std::cout << "-------------------->Calling PHY func1...<------------------------" << std::endl;
      return 0;
    }

    int func2(PhyPacket&& in, PhyPktVec& out )
    {

      std::cout << "Calling PHY func2..." << std::endl;
      return 0;
    }

    int func3(PhyPacket&& in, PhyPktVec& out )
    {

      std::cout << "Calling PHY func3..." << std::endl;
      return 0;
    }

    int func4(PhyPacket&& in, PhyPktVec& out )
    {

      std::cout << "Calling PHY func4..." << std::endl;
      return 0;
    }
};

