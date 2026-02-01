#pragma once
#include "graph.hpp"
#include "layout.hpp"
#include <cmath>
struct FruchtermanReingoldConf {
    float mx = 800;
    float my = 600;
    int max_iter = 500;
    float C = 0.5;
};

class FruchtermanReingold : public Layout {

  private:
    const FruchtermanReingoldConf& cfg_;
    float A_;
    float K_;
    float T_;
    int I_ = 0;

    float fa(float d, float k) { return (d * d) / k; }
    float fr(float d, float k) { return (k * k) / d; }
    float cool(float t) { return t * 0.99f; };
    void updatePositions(Graph& g);
    void computeAttractiveForces(Graph& g);
    void computeRepulsiveForces(Graph& g);

  public:
    ~FruchtermanReingold() override = default;
    explicit FruchtermanReingold(const FruchtermanReingoldConf& cfg) : cfg_(cfg) {}
    void apply(Graph& g) override;
};
