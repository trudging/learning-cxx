#include "../exercise.h"
#include <cstring>
// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = 1;
        // TODO: 填入正确的 shape 并计算 size
        for (int i = 0; i < 4; ++i) {
            shape[i] = shape_[i];
            size *= shape[i];
        }
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    }
    ~Tensor4D() {
        delete[] data;
    }

    // 为了保持简单，禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。
    Tensor4D &operator+=(Tensor4D const &others) {
        // TODO: 实现单向广播的加法
        // 计算总元素数量（size）用于循环
        unsigned int size = 1;
        for (int i = 0; i < 4; ++i) {
            size *= shape[i];
        }

        for (unsigned int i = 0; i < size; ++i) {
            // 计算当前一维索引 i 对应的 4维坐标 (n, c, h, w)
            // 假设 shape 是 [d0, d1, d2, d3] (batch, channel, height, width)
            unsigned int current_idx = i;
            unsigned int idx[4]; // 存储分解后的 4D 坐标
            
            // 使用除法和取模进行坐标转换（Stride 计算的逆过程）
            // 注意：通常 Tensor 存储是行优先（Row Major），即最后一个维度变化最快
            // 这里我们手动展开计算，或者利用 stride
            
            // 为了简化，我们边计算坐标边计算 others 的各种 broadcast 索引
            // 但最通用的方法是先算出坐标
            unsigned int temp = i;
            for (int obj = 3; obj >= 0; --obj) {
                idx[obj] = temp % shape[obj];
                temp /= shape[obj];
            }

            // 计算 others 的线性索引
            // 广播规则：如果 others.shape[dim] == 1，则该维度的坐标始终取 0
            // 否则，取和 this 一样的坐标 idx[dim]
            unsigned int others_linear_index = 0;
            unsigned int others_stride = 1;
            
            for (int k = 3; k >= 0; --k) {
                // 如果 others 在该维度是 1，则坐标始终是 0，对线性索引的贡献是 0 * stride
                // 如果 others 在该维度不是 1，则必须和 this 一样（题目假设），坐标是 idx[k]
                if (others.shape[k] != 1) {
                    others_linear_index += idx[k] * others_stride;
                }
                others_stride *= others.shape[k];
            }

            data[i] += others.data[others_linear_index];
        }

        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}
