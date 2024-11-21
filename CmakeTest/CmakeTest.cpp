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
const double GOLDEN_RATIO = (1 + sqrt(5)) / 2;
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
double f(const string& expression, const double& xValue) {
    exprtk::expression<double> expr;
    exprtk::symbol_table<double> symbolTable;
    double x = xValue; 

    symbolTable.add_variable("x", x);
    symbolTable.add_constants();
    expr.register_symbol_table(symbolTable);

    exprtk::parser<double> parser;
    if (!parser.compile(expression, expr)) {
        cout << "Error parsing expression: " << parser.error() << endl;
        return 0.0; 
    }

    return expr.value(); 
}
double derivative(double value, const string& expr) {
    double h = 1e-6;
    return (f(expr, value + h) - f(expr, value - h))/(2*h);
}
double secondDerivative(double value, const string& expr) {
    double h = 1e-6;
    return (f(expr, value + h) -2*f(expr,value)+ f(expr, value - h)) / (h * h);
}
vector<double> newtonMethod(double delta, double x0, const string& expr) {
    vector<double> answer = {};
    int count = 0;
    double xk = x0;

    while (true) {
        double firstDerivative = derivative(xk, expr);
        double sDerivative = secondDerivative(xk, expr);
        if (abs(sDerivative) < 1e-12) {
            cout << "Denominator too small, stopping." << endl;
            break;
        }
        double xk1 = xk - firstDerivative / sDerivative;
        if (abs(xk1 - xk) <= delta) {
            answer.push_back(xk1);              //x_min
            answer.push_back(f(expr, xk1));     //f(x_min)
            answer.push_back(count);           //iteration count
            return answer;
        }

        xk = xk1; 
    }
    return { 0, 0, 0 };
}

vector <double>goldenSection(double delta, double a, double b, int mode, const string& expr) {
    vector <double> answer = {};
    int count = 0;
    double x1 = b - (b - a) / GOLDEN_RATIO;
    double x2 = a + (b - a) / GOLDEN_RATIO;
    double y1 = f(expr,x1); double y2 = f(expr,x2);
    if (mode == 0) { //min
        while (abs(a - b) >= delta) {
            if (y1 >= y2) {
                a = x1; x1 = x2; x2 = a + (b - a) / GOLDEN_RATIO; y1 = f(expr,x1); y2 = f(expr,x2);
            }
            else {
                b = x2; x2 = x1; x1 = b - (b - a) / GOLDEN_RATIO; y1 = f(expr,x1); y2 = f(expr,x2);
            }
            count++;
        }
    }
    else if (mode == 1) { //max
        while (abs(a - b) >= delta) {
            if (y1 <= y2) {
                a = x1; x1 = x2; x2 = a + (b - a) / GOLDEN_RATIO; y1 = f(expr,x1); y2 = f(expr,x2);
            }
            else {
                b = x2; x2 = x1; x1 = b - (b - a) / GOLDEN_RATIO; y1 = f(expr,x1); y2 = f(expr,x2);
            }
            count++;
        }
    }
    else {
        cout << "improper mode was passed";
        return { 0,0,0 };
    }

    double x = (a + b) / 2;
    answer.push_back(x); answer.push_back(f(expr,x)); answer.push_back(count);
    return answer;
}
int main() {
    // Boilerplate for GLFW and ImGui initialization
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
    ImGui::StyleColorsDark();
    (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // State variables
    char functionBuffer[128] = "sin(x)";
    double xMin = -10.0, xMax = 10.0;
    double a = 0, b = 1, x0 = 0;
    double delta = 0.001;
    string goldenOutput1, goldenOutput2, newtonOutput1;
    vector<double> xValues, yValues;

    //points for scatter plot
    double highlightGoldenXMin = NAN, highlightGoldenYMin = NAN;
    double highlightGoldenXMax = NAN, highlightGoldenYMax = NAN;
    double highlightNewtonX = NAN, highlightNewtonY = NAN;

    //flags to prevent excessive plotting
    bool needsPlotUpdate = true;
    bool needsOptimizationUpdate = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main Function Plotter Window
        ImGui::Begin("Function Plotter");
        ImGui::InputText("Function (use 'x' as the variable)", functionBuffer, sizeof(functionBuffer));
        ImGui::InputDouble("X Min", &xMin);
        ImGui::InputDouble("X Max", &xMax);
        ImGui::InputDouble("Left Bracket", &a);
        ImGui::InputDouble("Right Bracket", &b);
        ImGui::InputDouble("Initial X Guess", &x0);

        if (xMin >= xMax) {
            ImGui::Text("Error: X Min must be less than X Max!");
        }
        if (ImGui::Button("Plot")) {
            needsPlotUpdate = true;
        }

        if (ImGui::Button("Run Optimization")) {
            needsOptimizationUpdate = true;
        }

        ImGui::End();


        if (needsPlotUpdate) {
            xValues.clear();
            yValues.clear();
            int numPoints = 500; // Line resolution
            double step = (xMax - xMin) / (numPoints - 1);
            for (int i = 0; i < numPoints; ++i) {
                xValues.push_back(xMin + i * step);
            }
            yValues = evaluateExpression(functionBuffer, xValues);
            needsPlotUpdate = false;
        }

        if (needsOptimizationUpdate) {
            auto goldenResult1 = goldenSection(delta, a, b, 0, functionBuffer);
            auto goldenResult2 = goldenSection(delta, a, b, 1, functionBuffer);
            auto newtonResult1 = newtonMethod(delta, x0, functionBuffer);

            highlightGoldenXMin = goldenResult1[0];
            highlightGoldenYMin = goldenResult1[1];
            highlightGoldenXMax = goldenResult2[0];
            highlightGoldenYMax = goldenResult2[1];
            highlightNewtonX = newtonResult1[0];
            highlightNewtonY = newtonResult1[1];

            // Formatting results
            goldenOutput1 = "Golden Section [Mode 0]:\nMin x = " + to_string(goldenResult1[0]) +
                ", f(x) = " + to_string(goldenResult1[1]) +
                ", Iterations = " + to_string((int)goldenResult1[2]);

            goldenOutput2 = "Golden Section [Mode 1]:\nMax x = " + to_string(goldenResult2[0]) +
                ", f(x) = " + to_string(goldenResult2[1]) +
                ", Iterations = " + to_string((int)goldenResult2[2]);

            newtonOutput1 = "Newton Method:\nExtremum x = " + to_string(newtonResult1[0]) +
                ", f(x) = " + to_string(newtonResult1[1]) +
                ", Iterations = " + to_string((int)newtonResult1[2]);

            needsOptimizationUpdate = false; // Optimization results are up-to-date
        }

        ImGui::Begin("Function Plot");
        // Plot Window
        if (ImPlot::BeginPlot("Function Plot")) {
            if (!xValues.empty() && !yValues.empty()) {
                ImPlot::PlotLine("f(x)", xValues.data(), yValues.data(), xValues.size());
            }
            if (!isnan(highlightGoldenXMin) && !isnan(highlightGoldenYMin)) {
                ImPlot::PlotScatter("Golden Min [Mode 0]", &highlightGoldenXMin, &highlightGoldenYMin, 1);
            }
            if (!isnan(highlightGoldenXMax) && !isnan(highlightGoldenYMax)) {
                ImPlot::PlotScatter("Golden Max [Mode 1]", &highlightGoldenXMax, &highlightGoldenYMax, 1);
            }
            if (!isnan(highlightNewtonX) && !isnan(highlightNewtonY)) {
                ImPlot::PlotScatter("Newton Extremum", &highlightNewtonX, &highlightNewtonY, 1);
            }
            ImPlot::EndPlot();
        }
        else
            cout << "begin plot failed" << endl;
        ImGui::End();

        // Optimization Results 
        ImGui::Begin("Optimization Results");
        ImGui::TextWrapped("%s", goldenOutput1.c_str());
        ImGui::Separator();
        ImGui::TextWrapped("%s", goldenOutput2.c_str());
        ImGui::Separator();
        ImGui::TextWrapped("%s", newtonOutput1.c_str());
        ImGui::End();

        // Render everything
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
