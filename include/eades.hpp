#pragma once
#include "graph.hpp"
#include "layout.hpp"
#include <cmath>

struct EadesConf {
    float mx = 800;
    float my = 600;
    int max_iter = 5000;
    float c1 = 2.0f;
    float c2 = 100.0f;
    float c3 = 1.0f;
    float c4 = 0.1f;
};

class Eades : public Layout {

  private:
    const EadesConf& cfg_;

    float repelForce(float d);
    float attractForce(float d);

  public:
    ~Eades() override = default;
    explicit Eades(const EadesConf& cfg) : cfg_(cfg) {}
    void apply(Graph& g) override;
};
