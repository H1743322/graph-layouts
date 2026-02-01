#pragma once
#include "graph.hpp"
#include "layout.hpp"
#include <cmath>
struct KamadaKawaiConf {
    float mx = 800;
    float my = 600;
    int max_iter = 500;
    int max_iter_2 = 100;
    float K = 1.0f;
    float multL = 1.0f;
};

class KamadaKawai : public Layout {

  private:
    const KamadaKawaiConf& cfg_;

    std::vector<std::vector<float>> L_;
    std::vector<std::vector<float>> K_;

    float fa(float d, float k) { return (d * d) / k; }
    float fr(float d, float k) { return (k * k) / d; }
    float cool(float t) { return t * 0.99f; };
    void computeLAndK(const std::vector<std::vector<float>>& dist, float L0, float k);
    void newtonRaphsonNode(Graph& g, int m);
    std::vector<float> computeEnergyAllNodes(Graph& g);
    float computeEnergy(Graph& g);

  public:
    ~KamadaKawai() override = default;
    explicit KamadaKawai(const KamadaKawaiConf& cfg) : cfg_(cfg) {}
    void apply(Graph& g) override;
};
