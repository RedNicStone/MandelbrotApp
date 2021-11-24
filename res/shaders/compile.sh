VULKAN_SDK_PATH=~/vulkan/vulkan-sdk/1.2.154.0/x86_64

$VULKAN_SDK_PATH/bin/glslc quad.vert -o vert_quad.spv

$VULKAN_SDK_PATH/bin/glslc iteration_simple.frag -o frag_iteration_simple.spv
$VULKAN_SDK_PATH/bin/glslc iteration_orbit_trap.frag -o frag_iteration_orbit_trap.spv
$VULKAN_SDK_PATH/bin/glslc shading_rainbow.frag -o frag_shading_rainbow.spv
