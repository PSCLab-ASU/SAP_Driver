#include <iostream>
#include <ranges>
#include "include/utils.h"
#include "include/layers/base_layer.h"
#include "include/pipeline_data.h"

#pragma once
struct SessionPacket : public BasePacket
{
  using BasePacket::get_base;
  //restriction interface view
  SessionPacket( typename BasePacket::type base )
  : BasePacket( base )
  {}

};

template< typename InputType=NullType>
class SessionLayer : public base_layer<SessionLayer<InputType>, SessionPacket >
{
  public :
    using InputType_t   = InputType;
    using SessionPktVec = std::vector<SessionPacket>;

    SessionLayer()
    {
      using namespace common_layer_cmds;
      using class_type = SessionLayer<InputType>;

      const auto FromApp = Pipelineflow::FromApp;
      const auto FromPhy = Pipelineflow::FromPhy; 

      std::cout << "0)Registrating SessionLayer functions" << std::endl;
      this->template register_cmd<FromApp>(self, &class_type::func0);
      std::cout << "1)Registrating SessionLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command1, &class_type::func1);
      std::cout << "2)Registrating SessionLayer functions" << std::endl;
      this->template register_cmd<FromApp>(command2, &class_type::func2);
      std::cout << "3)Registrating SessionLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command3, &class_type::func3);
      std::cout << "4)Registrating SessionLayer functions" << std::endl;
      this->template register_cmd<FromPhy>(command4, &class_type::func4);
      std::cout << "5)Registrating SessionLayer functions" << std::endl;
    }

    template<std::ranges::input_range R>
    SessionLayer(R& r)
    : SessionLayer()
    {
      std::cout << "SessionLayer ctor " << std::endl;
    }

    template<std::ranges::input_range R>
    SessionLayer(R& r, InputType& in)
    : SessionLayer()
    {
      std::cout << "SessionLayer ctor(in) " << std::endl;
    }

    int func0(SessionPacket&& in, SessionPktVec& out )
    {
      std::cout << "Calling Session func0..." << std::endl;
      return 0;
    }

    int func1(SessionPacket&& in, SessionPktVec& out )
    {
      std::cout << "Calling Session func1..." << std::endl;
      return 0;
    }

    int func2(SessionPacket&& in, SessionPktVec& out )
    {

      std::cout << "Calling Session func2..." << std::endl;
      return 0;
    }

    int func3(SessionPacket&& in, SessionPktVec& out )
    {

      std::cout << "Calling Session func3..." << std::endl;
      return 0;
    }

    int func4(SessionPacket&& in, SessionPktVec& out )
    {

      std::cout << "Calling Session func4..." << std::endl;
      return 0;
    }
};

