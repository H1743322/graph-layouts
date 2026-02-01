#pragma once
#include "graph.hpp"
#include "layout.hpp"

struct HarellKorenConf {
    float mx = 800.0f;
    float my = 600.0f;
    int max_iter = 4;
    int rad = 7;
    int ratio = 3;
    int min_size = 10;
    float K = 1.0f;
};

struct NodeEnergy {
    float energy;
    float dx = 0.0f;
    float dy = 0.0f;
    int node = -1;
    bool operator<(const NodeEnergy& other) const { return energy < other.energy; }
};

class HarellKoren : public Layout {

  private:
    const HarellKorenConf& cfg_;

    std::vector<int> centers_;
    std::vector<std::vector<float>> dist_;
    std::vector<std::vector<float>> L_;
    std::vector<std::vector<float>> K_;

  public:
    explicit HarellKoren(const HarellKorenConf& cfg) : cfg_(cfg) {}
    ~HarellKoren() override = default;
    void apply(Graph& g) override;

    std::vector<int> kCenters(const Graph& g, const std::vector<std::vector<float>>& dist, size_t k);
    void computeLAndK(const std::vector<std::vector<float>>& dist, float L0, float k);

    float computeRadius(const std::vector<int>& centers, const std::vector<std::vector<float>>& dist,
                        float Rad);

    void localLayout(Graph& g, const std::vector<std::vector<float>>& dist, float radius);

    NodeEnergy computeDeltaK(const Graph& g, int v, const std::vector<std::vector<float>>& dist,
                             const std::vector<int>& neighborhood);
    std::vector<std::vector<int>> computeKNeighborhoods(const Graph& g,
                                                        const std::vector<std::vector<float>>& dist, int k);

    std::vector<float> computeEnergyDeltaAllNodes(Graph& g, const std::vector<std::vector<float>>& d,
                                                  const std::vector<std::vector<int>>& neighborhoods);

    void noise(Graph& g, const std::vector<int>& centers, const std::vector<std::vector<float>>& dist,
               const size_t V);
};
