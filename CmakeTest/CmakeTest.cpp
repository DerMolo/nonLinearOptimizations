#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "exprtk.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

// evaluating expression using ExprTk
vector<double> evaluateExpression(const string& expression, const vector<double>& xValues) {
    vector<double> yValues;
    exprtk::expression<double> expr;
    exprtk::symbol_table<double> symbolTable;
    double x;

    symbolTable.add_variable("x", x);
    symbolTable.add_constants();
    expr.register_symbol_table(symbolTable);

    exprtk::parser<double> parser;
    if (!parser.compile(expression, expr)) {
        cout << "Error parsing expression: " << parser.error() << endl;
        yValues.assign(xValues.size(), 0.0); // Fill with zeros on error
        return yValues;
    }

    for (const double& xi : xValues) {
        x = xi;
        yValues.push_back(expr.value());
    }
    return yValues;
}

int main() {
    //boiler plate plotter and window initialization 
        if (!glfwInit())
            throw runtime_error("Failed to initialize GLFW");

        GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui + ImPlot + ExprTk Demo", NULL, NULL);
        if (!window)
            throw runtime_error("Failed to create GLFW window");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        char functionBuffer[128] = "sin(x)";
        double xMin = -10.0, xMax = 10.0;
        std::vector<double> xValues, yValues;

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Function Plotter");
            ImGui::InputText("Function (use 'x' as the variable)", functionBuffer, sizeof(functionBuffer));
            ImGui::InputDouble("X Min", &xMin);
            ImGui::InputDouble("X Max", &xMax);

            if (xMin >= xMax) {
                ImGui::Text("Error: X Min must be less than X Max!");
            }
            else if (ImGui::Button("Plot")) {
                xValues.clear();
                yValues.clear();
                int numPoints = 1000;
                double step = (xMax - xMin) / (numPoints - 1); //resolution of the curve
                for (int i = 0; i < numPoints; ++i) {
                    xValues.push_back(xMin + i * step);
                }
                yValues = evaluateExpression(functionBuffer, xValues);
            }

            if (ImPlot::BeginPlot("Function Plot")) {
                if (!xValues.empty() && !yValues.empty()) {
                    ImPlot::PlotLine("f(x)", xValues.data(), yValues.data(), xValues.size());
                }
                ImPlot::EndPlot();
            }
            ImGui::End();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImPlot::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
}
