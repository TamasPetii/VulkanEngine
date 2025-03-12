%VULKAN_SDK%\Bin\glslc.exe Shader.vert -o Shader-vs.spv
%VULKAN_SDK%\Bin\glslc.exe Shader.frag -o Shader-fs.spv
%VULKAN_SDK%\Bin\glslc.exe Gradient.comp -o Gradient-cp.spv
%VULKAN_SDK%\Bin\glslc.exe GradientPush.comp -o GradientPush-cp.spv
%VULKAN_SDK%\Bin\glslc.exe Triangle.vert -o Triangle-vs.spv
%VULKAN_SDK%\Bin\glslc.exe Triangle.frag -o Triangle-fs.spv
pause