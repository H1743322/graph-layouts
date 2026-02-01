#pragma once
#include "eades.hpp"
#include "fruchterman_reingold.hpp"
#include "graph.hpp"
#include "graph_loader.hpp"
#include "harell_koren.hpp"
#include "imgui.h"
#include "kamada_kawai.hpp"
#include "walshaw.hpp"
#include <memory>
#include <string>
#include <vector>

// TODO: strategy design patter for layout ui?
class UIManager {
  public:
    float nodeSize = 5.0f;

    // Layout
    int currentLayout = 0;
    const char* layoutItems[5] = {"Fruchterman", "Harel-Koren", "Walshaw", "Kamda-Kawai", "Eades"};
    FruchtermanReingoldConf fruchtermanConfig;
    HarellKorenConf harelConfig;
    WalshawConf walshawConfig;
    KamadaKawaiConf kamadaKawaiConfig;
    EadesConf eadesConfig;
    bool initialized = false;

    // Loader
    const char* graphSources[5] = {
        "File .SRC", "File .MMX", "Grid", "Sierpinksi", "Torus",
    };
    int currentGraphSource = 1;
    std::string graphPathSrc = "graphs/fe_3elt.src";
    std::string graphPathMtx = "graphs/crack.mtx";
    int gridWidth = 10, gridHeight = 10;
    int sierpinksiDepth = 2;

    void render(Graph& graph, float W, float H) {
        ImGui::Begin("Graph Controls");

        if (ImGui::BeginTabBar("GraphTabs")) {

            if (ImGui::BeginTabItem("Info")) {
                renderFPS();
                renderGraphInfo(graph);
                renderNodeSize();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Loader")) {
                renderGraphLoader(graph, W, H);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Layout")) {
                renderLayout(graph, W, H);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

  private:
    void renderFPS() {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Separator();
    }

    void renderGraphInfo(Graph& graph) {
        ImGui::Text("Nodes: %zu, Edges: %zu", graph.nodes.size(), graph.getEdgeCount());
        ImGui::Separator();
    }

    void renderNodeSize() {
        ImGui::SliderFloat("Node Size", &nodeSize, 0.0f, 100.0f);
        ImGui::Separator();
    }

    void renderGraphLoader(Graph& graph, float W, float H) {
        // if (!ImGui::CollapsingHeader("Graph Build", ImGuiTreeNodeFlags_DefaultOpen))
        //     return;

        ImGui::Combo("Sources Name", &currentGraphSource, graphSources, IM_ARRAYSIZE(graphSources));
        switch (currentGraphSource) {
        case 0:
            ImGui::InputText("File Path", &graphPathSrc[0], graphPathSrc.size() + 20);
            break;
        case 1:
            ImGui::InputText("File Path", &graphPathMtx[0], graphPathMtx.size() + 20);
            break;
        case 2:
            ImGui::SliderInt("Width Nodes", &gridWidth, 1, 100);
            ImGui::SliderInt("Height Nodes", &gridHeight, 1, 100);
            break;
        case 3:
            ImGui::SliderInt("Depth", &sierpinksiDepth, 1, 20);
            break;
        case 4:
            ImGui::SliderInt("Width Nodes", &gridWidth, 1, 200);
            ImGui::SliderInt("Height Nodes", &gridHeight, 1, 200);
            break;
        }

        if (ImGui::Button("Load Graph")) {
            graph.clear();
            switch (currentGraphSource) {
            case 0:
                loadSotch(graph, graphPathSrc);
                break;
            case 1:
                loadMtx(graph, graphPathMtx);
                break;
            case 2:
                buildGrid(graph, gridWidth, gridHeight);
                break;
            case 3:
                buildSierpinski(graph, sierpinksiDepth);
                break;
            case 4:
                buildTorus(graph, gridWidth, gridHeight);
                break;
            }
            graph.randomizePos(W, H);
        }
    }

    void renderLayout(Graph& graph, float W, float H) {
        // if (!ImGui::CollapsingHeader("Layout", ImGuiTreeNodeFlags_DefaultOpen))
        //     return;

        ImGui::Combo("Layout Name", &currentLayout, layoutItems, IM_ARRAYSIZE(layoutItems));

        if (!initialized) {
            fruchtermanConfig.mx = W;
            fruchtermanConfig.my = H;
            harelConfig.mx = W;
            harelConfig.my = H;
            walshawConfig.mx = W;
            walshawConfig.my = H;
            kamadaKawaiConfig.mx = W;
            kamadaKawaiConfig.my = H;
            eadesConfig.mx = W;
            eadesConfig.my = H;
            initialized = true;
        }

        switch (currentLayout) {
        case 0:
            renderFruchterman();
            break;
        case 1:
            renderHarelKoren();
            break;
        case 2:
            renderWalshaw();
            break;
        case 3:
            renderKamadaKawai();
            break;
        case 4:
            renderEades();
            break;
        }

        if (ImGui::Button("Apply Layout")) {
            applyCurrentLayout(graph);
        }
    }

    void renderFruchterman() {
        ImGui::Text("Fruchterman Parameters");
        ImGui::InputFloat("Width", &fruchtermanConfig.mx);
        ImGui::InputFloat("Height", &fruchtermanConfig.my);
        ImGui::InputFloat("C", &fruchtermanConfig.C);
        ImGui::InputInt("Iterations", &fruchtermanConfig.max_iter);
    }

    void renderHarelKoren() {
        ImGui::Text("Harel-Koren Parameters");
        ImGui::InputFloat("Width", &harelConfig.mx);
        ImGui::InputFloat("Height", &harelConfig.my);
        ImGui::InputFloat("K", &harelConfig.K);
        ImGui::InputInt("Radius", &harelConfig.rad);
        ImGui::InputInt("Ratio", &harelConfig.ratio);
        ImGui::InputInt("Iterations", &harelConfig.max_iter);
        ImGui::InputInt("Min Size", &harelConfig.min_size);
    }

    void renderWalshaw() {
        ImGui::Text("Walshaw Parameters");
        ImGui::InputFloat("Width", &walshawConfig.mx);
        ImGui::InputFloat("Height", &walshawConfig.my);
        ImGui::SliderInt("Iterations", &walshawConfig.max_iter, 50, 1000);
        ImGui::InputFloat("C", &walshawConfig.C);
        ImGui::InputFloat("Tol", &walshawConfig.tol);
    }

    void renderKamadaKawai() {
        ImGui::Text("Kamda-Kawai Parameters");
        ImGui::InputFloat("Width", &kamadaKawaiConfig.mx);
        ImGui::InputFloat("Height", &kamadaKawaiConfig.my);
        ImGui::InputInt("Iterations", &kamadaKawaiConfig.max_iter);
        ImGui::InputInt("Iterations 2", &kamadaKawaiConfig.max_iter_2);
        ImGui::InputFloat("K", &kamadaKawaiConfig.K);
        ImGui::InputFloat("Length Mult", &kamadaKawaiConfig.multL);
    }

    void renderEades() {
        ImGui::Text("Eades Parameters");
        ImGui::InputFloat("Width", &eadesConfig.mx);
        ImGui::InputFloat("Height", &eadesConfig.my);
        ImGui::InputInt("Iterations", &eadesConfig.max_iter);
        ImGui::InputFloat("C1", &eadesConfig.c1);
        ImGui::InputFloat("C2", &eadesConfig.c2);
        ImGui::InputFloat("C3", &eadesConfig.c3);
        ImGui::InputFloat("C4", &eadesConfig.c4);
    }

    // TODO: Threads
    void applyCurrentLayout(Graph& graph) {
        std::unique_ptr<Layout> layout;
        switch (currentLayout) {
        case 0:
            layout = std::make_unique<FruchtermanReingold>(fruchtermanConfig);
            break;
        case 1:
            layout = std::make_unique<HarellKoren>(harelConfig);
            break;
        case 2:
            layout = std::make_unique<Walshaw>(walshawConfig);
            break;
        case 3:
            layout = std::make_unique<KamadaKawai>(kamadaKawaiConfig);
            break;
        case 4:
            layout = std::make_unique<Eades>(eadesConfig);
            break;
        }
        if (layout)
            layout->apply(graph);
    }
};
