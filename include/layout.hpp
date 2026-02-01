#pragma once
#include "graph.hpp"

const float EPSILON = 1e-4f;


class Layout {

  public:
    virtual void apply(Graph& g) = 0;
    virtual ~Layout() = default;
};
