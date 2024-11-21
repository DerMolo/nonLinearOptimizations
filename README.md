Follow these instructions within powershell 
Prerequisite installations: 
- git bash
- Cmake https://cmake.org/download/
  
library installation procedure: 
cd C:\Users\"yourusername"\source\repos
git clone https://github.com/DerMolo/nonLinearOptimizations

Downloading libraries into the lib folder: 
cd nonLinearOptimizations/libs

- git clone https://github.com/ArashPartow/exprtk.git exprtk
- git clone https://github.com/glfw/glfw.git glfw
- git clone https://github.com/epezent/implot.git implot
- git clone https://github.com/ocornut/imgui.git imgui

Building glfw: 
cd C:\Users\"yourusername"\source\repos\nonLinearOptimizations\libs\glfw
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"

Now run glfw.sln as adminstrator in visual studios 2022, and build the project.

AFterward, CmakeTest.cpp should be executable once successfully built.
