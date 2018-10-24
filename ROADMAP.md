## Project: roadmap, licenses and documentation.
- [x] 01 Project structure, build system, makefiles.
- [ ] 00 Licenses (& editor-licenses).
- [ ] 00 Documentation, conventions and style.

## Editor
- [x] app : window, input and docks
- [x] gizmo
- [x] widgets
- [x] 00 Bring back IMGUI DebugDraw 3D.
- [ ] 00 Bring back OSC/UDP messages.
- [ ] 00 Bring back MPEG2/debug recording.
- [ ] 00 Link with import pipeline.
- [ ] 00 Scriptable windows (plus Remote API)
- [ ] 00 IPC mem sharing.
- [ ] 00 Specify final JSON/Object notation.
- [ ] 00 Object/Hierarchy widget.
- [ ] 00 Property widget.
- [ ] 00 Timeline and data tracks.
- [ ] 00 Toolbar: button, on/off buttons, radio buttons (one).
- [ ] `kit.vcs` : versioning and data journaling.
- [ ] foreign renderer -> send: render(mat4 vr[2], int quality), recv: render.jpg
- [ ] persistence
- [ ] serialization, exporter (oop.json, ecs.json)
- [ ] hitboxes
- [ ] world, objects, and dependencies
- [ ] messages and events
- [ ] pipeline, importer
- [ ] foreign input -> send: input(tap1,tap2,mx,my,mb,key[256]), recv: world.diff file
- [ ] foreign raycasts -> send: ray(p3,dir), recv: ray.hit file
- [ ] foreign logic -> send: tick(dt), recv: world.diff file
- [ ] every tab is an OS app with FILE* commands tick,step,edit,init,quit,draw,


## Import
- [ ] 00 Bin/Human File formats, hashing, encryption and compression.
- [ ] 01 Data pipeline and asset journaling.

## Core: standard library, runtime and containers.
- [x] `crt` : C runtime and entrypoint.
---
- [ ] `kit.mem` : heap memory & memory-mapping.
- [x] `crt.bit` : bit packing and endianness.
- [ ] `crt.ios` : io streams.
- [ ] `kit.dbs` : databases and storage.
- [ ] `kit.cfg` : configuration files.
- [x] `crt.abi` : symbol linkage and visibility.
- [x] `stl.mem` : memory allocators
- [x] `stl.vec` : dynamic buffers.
- [x] `stl.str` : dynamic strings.
- [x] `std.arg` : arguments and cmdline.
- [x] `std.clk` : time and date.
- [x] `std.crc` : crc and hashing.
- [x] `std.dir` : directories, files and listings.
- [x] `std.dll` : dynamic library loading.
- [x] `std.dsk` : disk and space.
- [x] `std.env` : environment and variables.
- [x] `std.err` : errors, asserts and failures.
- [x] `std.exp` : expression evaluator.
- [x] `std.log` : logging and traces.
- [x] `std.mtx` : mutexes and locks.
- [x] `std.rnd` : randomness and entropy.
- [x] `std.ser` : serialization.
- [x] `std.sym` : symbols, demangling and stacktraces.
- [x] `std.thd` : threading and concurrency.
- [x] `std.tls` : thread-local storage.
- [x] `std.tst` : unit-tests and suites.
- [x] `std.wtf` : compiler, platform and os macro hell.
- [x] `std.tty` : terminal.

## Framework
- [ ] 00 Game loop.
- [ ] 04 Scripting, bindings and modules.
- [ ] 09 Hot-reloading and watchdogs.
- [ ] 10 Timers, easings and interpolation.
- [ ] 08 Digital signals, buffering and polling.
- [ ] 11 HID interface: bindings, digital/analog inputs, filters, gestures, chords and combos.
- [ ] 12 Inspection: debugger, console, leaks, cvars, registers, callstack, gameobjects and profiling.
- [ ] 30 2D/3D audio & recoding.
- [ ] 31 Full Motion Video and integrated cinematics.
- [ ] `kit.vfs` : virtual filesystem.
- [ ] `kit.wnd` : windowing.
- [ ] `kit.net` : network.
- [x] `std.hlt` : crash handling and reports.
- [x] `std.lin` : linear algebra.
- [x] `std.dlg` : user dialogs.
- [x] `std.dlg` : console.

## Renderer
- [ ] 16 2D/2.5D/3D Renderer, FBOs, PostFX, FBOs.
- [ ] 18 LOD and frustum culling.
- [ ] 19 Fonts, truetypes, L10N, i18n, subtitles and unicode.
- [ ] 20 2D debug view, debug primitives, debug UI.
- [ ] 21 Procedural content, brushes, voxels, noise and CSG.
- [ ] 22 Static meshes, skyboxes, skydomes, cubemaps.
- [ ] 23 Spritesheets, GIFs, Billboards, 2D Decals, Backgrounds.
- [ ] 24 Particles, emitters, trails and paths.

## Gameplay
- [ ] 17 Arcballs, cameras and 3D navigation.
- [ ] 25 Navpoints, navmesh traversal and pathfinding.
- [ ] 26 Spines, skeletal animation, blending, skinning, IK.
- [ ] 27 H/FSM, AI and blend zones.
- [ ] 28 Game flow, HUD and UI.
- [ ] 29 Dialogue, quests and inventory.
- [ ] `kit.evt` : events.
- [ ] `std.loc` : locales, units and conversions.
- [ ] `kit.i18` : internationalization & localization.
- [ ] `kit.dev` : dev menus and cheats.
- [ ] `kit.phy` : physics.
- [ ] `kit.cup` : trophies, scoring, quests, dialogue and inventory.
- [ ] `kit.lod` : LODs and frustum culling.
- [ ] `kit.lvl` : level loading and world streaming.
- [ ] `kit.pcg` : procedural content generators.
- [ ] `kit.pro` : procedural, bouyancy, wind, csg and noise.

## World Server: virtual CPU, messaging, reflection, gameobjs and scenes.
- [ ] 05 Server/client architecture.
- [ ] 06 Message pipeline and serialization.
- [ ] 07 Entity/Component/Systems.
- [ ] 13 Volumes, collisions, regions and triggers.
- [ ] 14 Trees, scenegraphs and spatial partioning.
- [ ] 15 World streaming and level loading.
- [ ] `kit.reg` : asset registry.
- [ ] `kit.pkg` : packaging and dependencies.
- [ ] `kit.p2p` : distribution, cloud, consensus. cloud services .
- [ ] `kit.soc` : social.
- [ ] sharding, clustering.
- [ ] reckoning, interpolation, lag compensation, ...


## Old 00 - Apps: Applications, layers and services.
- [ ] `app.101` : game config
- [ ] `app.102` : game update
- [ ] `app.107` : game server
---
- [ ] `app.100` : game launch
- [ ] `app.101` : game config
- [ ] `app.102` : game update
- [ ] `app.103` : game splash
- [ ] `app.106` : game client
--
- [ ] `app.104` : game notify
- [ ] `app.110` : game cooker
- [ ] `app.105` : game viewer
- [ ] `app.108` : game editor
- [ ] `app.109` : game studio

## Old 01 - Core: runtime, containers and standard library.
- [ ] `stl` :  standard containers and data structures.
- [ ] `stl.buf` : fixed buffers.
- [ ] `stl.deq` : double-ended queue.
- [ ] `stl.lst` : doubly linked lists.
- [ ] `stl.map` : associative maps.
- [ ] `stl.set` : sets.
- [ ] `stl.var` : variants.
- [ ] `stl.tea` : trees.
---
- [ ] `std:`  standard lib
- [ ] `std.cor` : coroutines and greenlets.
- [ ] `std.def` : definitions, types and constants.
- [ ] `std.geo` : geometry.
- [ ] `std.hot` : hot-patching and api hooks.
- [ ] `std.kit` : kits and native plugins.
- [ ] `std.lrp` : lerp and curve interpolators.
- [ ] `std.num` : numerics, limits and math.
- [ ] `std.pkg` : packages and dependencies.
- [ ] `std.txt` : text manipulation.
- [ ] `std.uid` : unique ids.
- [ ] `std.utf` : utf.

## Old 02 - Server
- [ ] `cpu` :  virtual cpus, logic machines and modules (c++, dlls, scripts)
- [ ] `cpu.mod` : modules and layers.
- [ ] `cpu.bnd` : binding.
- [ ] `cpu.src` : preprocess/directives, compile/paths, link.
- [ ] `cpu.mem` : memory poke/peek.
- [ ] `cpu.run` : simulation rates.
- [ ] `cpu.tck` : ticks, timing.
- [ ] `msg` :  messages and serialization
- [ ] `msg.pbc` : protos and schemas
- [ ] `msg.evt` : events and signal.
- [ ] `msg.tok` : token list.
- [ ] `msg.chn` : channel abstraction.
---
- [ ] `obj` :  gameobjs and ecs.
- [ ] `obj.uid` : entities.
- [ ] `obj.res` : data components.
- [ ] `obj.sys` : code systems.
---
- [ ] `scn` : scenes and trees of objs.
- [ ] `lvl` : levels and trees of scenes.
- [ ] `gme` : games and trees of levels.

