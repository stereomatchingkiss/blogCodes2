#ifndef TRANSFER_NET_HPP
#define TRANSFER_NET_HPP

#include <dlib/dnn.h>

namespace transfer{

using namespace dlib;

using net_00 = loss_multiclass_log<
fc<2,
dropout<relu<fc<128,
dropout<relu<fc<256,
input<matrix<float>
>>>>>>>>>;

using net_01 = loss_multiclass_log<
fc<2,
dropout<relu<fc<64,
dropout<relu<fc<128,
dropout<relu<fc<256,
input<matrix<float>
>>>>>>>>>>>>;

using net_02 = loss_multiclass_log<
fc<2,
dropout<relu<fc<256,
input<matrix<float>
>>>>>>;

using net_03 = loss_multiclass_log<
fc<2,
dropout<relu<fc<128,
input<matrix<float>
>>>>>>;

using net_04 = loss_multiclass_log<
fc<2,
dropout<relu<fc<32,
input<matrix<float>
>>>>>>;

using net_05 = loss_multiclass_log<
fc<2,
dropout<relu<fc<32,
dropout<relu<fc<16,
input<matrix<float>
>>>>>>>>>;

using net_06 = loss_multiclass_log<
fc<2,
dropout<relu<fc<64,
dropout<relu<fc<128,
input<matrix<float>
>>>>>>>>>;

using net_07 = loss_multiclass_log<
fc<2,
dropout<prelu<fc<128,
input<matrix<float>
>>>>>>;

using net_08 = loss_multiclass_log<
fc<2,
avg_pool_everything<
dropout<relu<fc<128,
input<matrix<float>
>>>>>>>;

using net_09 = loss_multiclass_log<
fc<2,
avg_pool_everything<
dropout<relu<fc<256,
input<matrix<float>
>>>>>>>;

using net_10 = loss_multiclass_log<
fc<2,
max_pool_everything<
dropout<relu<fc<128,
input<matrix<float>
>>>>>>>;

using net_11 = loss_multiclass_log<
fc<2,
dropout<
bn_con<relu<fc<256,dropout<
bn_con<relu<fc<256,dropout<
input<matrix<float>
>>>>>>>>>>>>;

}

#endif // TRANSFER_NET_HPP
