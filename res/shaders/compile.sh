VULKAN_SDK_PATH=~/vulkan/vulkan-sdk/1.2.154.0/x86_64

$VULKAN_SDK_PATH/bin/glslc quad.vert -o vert_quad.spv
$VULKAN_SDK_PATH/bin/glslc mandelbrot.frag -o frag_mandelbrot.spv

$VULKAN_SDK_PATH/bin/glslc shader.frag -o frag.spv
