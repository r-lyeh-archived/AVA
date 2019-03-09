Implicit rules:

  * Always call `glViewport` and `glScissor` at the beginning of each display list.
  * Don't call display list procedures outside of display lists.
  * Don't call display lists inside other display lists.

Examples: https://github.com/procedural/gl_portable_examples

---

Missing OpenGL pipeline state:
  
```c
glEnable(GL_SAMPLE_COVERAGE);
glSampleCoverage(value, invert);
```

Missing Vulkan pipeline state:

  * `VkPipelineInputAssemblyStateCreateInfo::primitiveRestartEnable`:
  
    For GL 3.1 or greater:
    ```c
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(index);
    ```
  
    For GLES 3.0 or greater:
    ```c
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    ```
  
  * `VkPipelineRasterizationStateCreateInfo::rasterizerDiscardEnable`:
  
    For GL 3.0 or greater and GLES 3.0 or greater:
    ```c
    glEnable(GL_RASTERIZER_DISCARD);
    ```

  * `VkPipelineRasterizationStateCreateInfo::depthBiasClamp`:
  
    For GL with GL_EXT_polygon_offset_clamp:
    ```c
    glPolygonOffsetClampEXT(depthBiasConstantFactor, depthBiasSlopeFactor, depthBiasClamp);
    ```
    
    For GLES there is no extension.
    
  * `VkPipelineMultisampleStateCreateInfo::rasterizationSamples`:
  * `VkPipelineMultisampleStateCreateInfo::pSampleMask`:
  
    For GL 3.2 or greater and GLES 3.1 or greater:
    ```c
    glEnable(GL_SAMPLE_MASK);
    glSampleMaski(rasterizationSamples, pSampleMask);
    ```
  
  * `VkPipelineMultisampleStateCreateInfo::sampleShadingEnable`:
  * `VkPipelineMultisampleStateCreateInfo::minSampleShading`:
  
    For GL 4.0 or greater and GL with GL_ARB_sample_shading and GLES 3.2 or greater and GLES with GL_OES_sample_shading:
    ```c
    glEnable(GL_SAMPLE_SHADING);
    glMinSampleShading(minSampleShading);
    ```
  
  * `VkPipelineDepthStencilStateCreateInfo::depthBoundsTestEnable`:
  * `VkPipelineDepthStencilStateCreateInfo::minDepthBounds`:
  * `VkPipelineDepthStencilStateCreateInfo::maxDepthBounds`:
  
    For GL with GL_EXT_depth_bounds_test:
    ```c
    glEnable(GL_DEPTH_BOUNDS_TEST_EXT);
    glDepthBoundsEXT(minDepthBounds, maxDepthBounds);
    ```
    
    For GLES there is no extension.
    
