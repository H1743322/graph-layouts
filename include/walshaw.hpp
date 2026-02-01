#pragma once
#include "graph.hpp"
#include "layout.hpp"
#include <cmath>

struct WalshawConf {
    float mx;
    float my;
    int max_iter = 100;
    float C = 1;
    float tol = 0.01f;
};
class Walshaw : public Layout {

  private:
    WalshawConf cfg_;
    float R_;

    constexpr float fg(const float x, const float w, const float k) {
        if (x <= R_)
            return -cfg_.C * w * k * k / x;
        else
            return 0.0f;
    }
    constexpr float fl(const float x, const float d, const float w, const float k) {
        return ((x - k) / d) - fg(x, w, k);
    }

    constexpr float fr(const float x, const float w, const float c, const float k) {
        return -c * w * k * k / x;
    }
    constexpr float fa(const float x, const float k) { return x * x / k; }


    constexpr float cool(const float t) { return t * 0.99f; }

  public:
    explicit Walshaw(const WalshawConf& cfg) : cfg_(cfg) {}
    ~Walshaw() override = default;
    void apply(Graph& g) override;
};
