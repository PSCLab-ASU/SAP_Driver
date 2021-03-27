#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <concepts>
#include <algorithm>
#include <assert.h>
#include <memory>
#include "include/utils.h"
#include "include/layers/base_layer.h"

#pragma once
 
namespace rv = std::ranges::views;
namespace rg = std::ranges;
 
template<typename Layer, typename InOut, rg::input_range R> requires rg::view<R>
class layer_view_interface : public rg::view_interface<layer_view_interface<Layer, InOut, R> >
{
    
    using InOutType = std::conditional_t<std::is_same_v<InOut, NullType>, NullType, InOut>;

    private:
        R                                         _base{};
        //base_layer<Layer>::layer_iterator<R>      _iter;
        std::shared_ptr<Layer>                    _layer_ptr;
        InOutType                                 _inout;

    public:
        layer_view_interface() = default;

        layer_view_interface(R base)
            : _base(base),
            _layer_ptr( std::make_shared<Layer>( _base ) )
        {
        }
        
        layer_view_interface(R base, InOut inout)
            : _base(base),
            _inout( inout ),
            _layer_ptr( std::make_shared<Layer>(_base, _inout ) )
        {

        }
        
        constexpr R base() const &
        {return _base;}

        constexpr R base() && 
        {return std::move(_base);}
        
        //constexpr typename base_layer<Layer>::layer_iterator<R> begin() const
        constexpr auto begin() const
        {
          std::cout << " -->Calling begin()" << std::endl;
          //return iter_;
          //return _layer_ptr->template begin<R, InOutType>(iter_); 
          return _layer_ptr->begin(_base); 
          //return iter_; 
        }

        //constexpr typename base_layer<Layer>::layer_iterator<R> end() const
        constexpr auto  end() const
        { 
          std::cout << " -->Calling end()" << std::endl;
          //return std::next(iter_, 0);
          return _layer_ptr->end(_base); 
          //return std::next(iter_, 5); 
        }
        
        constexpr auto size() const requires rg::sized_range<const R>
        { 
            const auto s = rg::size(_base);
            const auto c = 0;
            return s < c ? 0 : s - c;    
        }


};

 
template<typename Layer, typename In> 
struct custom_layer_range_adaptor_closure
{
    constexpr custom_layer_range_adaptor_closure() = default;

    constexpr custom_layer_range_adaptor_closure(In in)
    : _in(in)
    {}
 
    template <rg::viewable_range R>
    constexpr auto operator()(R && r) const
    {
      std::cout << "operator closure (...) " << std::endl;
      if constexpr( std::is_same_v<In, NullType> )
        return layer_view_interface<Layer, In, R>(std::forward<R>(r) );
      else
        return layer_view_interface<Layer, In, R>(std::forward<R>(r), _in );
    }

   std::conditional_t<std::is_same_v<In, NullType>, NullType, In> _in;
} ;
 
template<typename Layer> 
struct custom_layer_range_adaptor
{
    constexpr auto operator () ( )
    {
        return custom_layer_range_adaptor_closure<Layer, NullType>( );
    }  
 
    template<typename In>
    constexpr auto operator () (In in)
    {
        return custom_layer_range_adaptor_closure<Layer, In>(in);
    }   
};


//template <rg::viewable_range R, typename Layer, typename In>
template <typename R, typename Layer, typename In>
constexpr auto operator | (R&& rhs, custom_layer_range_adaptor_closure<Layer, In> const & lhs)
{
  std::cout << "operator | (...) " << std::endl;
  //static_assert(rg::viewable_range<R>, "Not a viewable Range");
  return lhs(std::forward<R>(rhs)) ;
}


