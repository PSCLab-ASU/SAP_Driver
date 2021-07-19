#include <ranges>
#include "include/utils.h"
#include "include/layers/layer_interface.h"
#include "include/layers/inout_layer.h"
#include "include/layers/pres_layer.h"
#include "include/layers/sess_layer.h"
#include "include/layers/trans_layer.h"
#include "include/layers/net_layer.h"
#include "include/layers/dlink_layer.h"
#include "include/layers/phy_layer.h"


namespace views
{
    using InOutL = custom_layer_range_adaptor<InOutLayer<NullType> >;

    using PresL  = custom_layer_range_adaptor<PresentationLayer<NullType> >;

    using SessL  = custom_layer_range_adaptor<SessionLayer<NullType> >;

    using TransL = custom_layer_range_adaptor<TransportLayer<NullType> >;

    using NetL   = custom_layer_range_adaptor<NetworkLayer<NullType> >;

    using DataL  = custom_layer_range_adaptor<DatalinkLayer<NullType> >;

    using PhyL   = custom_layer_range_adaptor<PhyLayer<NullType> >;
}

