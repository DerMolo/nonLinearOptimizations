Follow these instructions within powershell 
Prerequisite installations: 
- git bash
  
installation: 
cd C:\Users\"yourusername"\source\repos
git clone https://github.com/DerMolo/nonLinearOptimization

Downloading libraries into the lib folder: 
cd nonLinearOptimization/lib

Invoke-WebRequest -Uri "https://github.com/ocornut/imgui/archive/refs/heads/master.zip" -OutFile "imgui.zip"
Expand-Archive -Path "imgui.zip" -DestinationPath "imgui" -Force
Remove-Item "imgui.zip"

Invoke-WebRequest -Uri "https://github.com/epezent/implot/archive/refs/heads/master.zip" -OutFile "implot.zip"
Expand-Archive -Path "implot.zip" -DestinationPath "implot" -Force
Remove-Item "implot.zip"

Invoke-WebRequest -Uri "https://www.partow.net/downloads/exprtk.zip" -OutFile "exprtk.zip"
Expand-Archive -Path "exprtk.zip" -DestinationPath "exprtk" -Force
Remove-Item "exprtk.zip"

Invoke-WebRequest -Uri "https://github.com/glfw/glfw/releases/latest/download/glfw-3.3.8.bin.WIN64.zip" -OutFile "glfw.zip"
Expand-Archive -Path "glfw.zip" -DestinationPath "glfw" -Force
Remove-Item "glfw.zip"
