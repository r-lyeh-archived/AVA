glFast
------

<img width="800px" src="http://i.imgur.com/dQEm83w.gif" />

glFast is a public domain header-only C library that uses only 40 modern DSA AZDO OpenGL functions to draw geometry and post-process textures.

The contract:

 * SDL2, desktop OpenGL, Linux and Windows only. Doesn't support OS X, OpenGL ES or WebGL.
 * GPU memory is immutable. Once allocated you can't resize it, but you still can change its content.
 * No multithreaded or asynchronous CPU<->GPU interactions. No barriers and sync points except for glFinish call.
 * CPU sends data to GPU. GPU can't send data back to CPU, with an exception of reading textures from GPU to CPU.
 * Not all modern OpenGL extensions are used, only those which are supported on low-end hardware and Mesa 11.3.

7 data types:

 * `gpu_storage_t Storage`
 * `gpu_texture_t Texture`
 * `gpu_sampler_t Sampler`
 * `u32 Framebuffer`
 * `u32 Program`
 * `u32 ProgramPipeline`
 * `gpu_cmd_t DrawCommand`

5 state changes:

 * `glBindTextures()`
 * `glBindSamplers()`
 * `glBindFramebuffer()`
 * `glProgramUniform*()`
 * `glEnable(), glDisable()`

That's it. Check out the examples to see how to use the library.

Special thanks to Nicolas [@nlguillemot](https://github.com/nlguillemot) and Andreas [@ands](https://github.com/ands) for answering my OpenGL questions!

