# pipeline

Background daemon scans over asset directories looking for work to do (like texture atlasing).
Whenever it sees work to be done, it kicks off and does it.
Then network messages (or disk signals) are broadcasted for notification purposes (for those games that dont monitor disk changes).
Then sleeps and repeats.

# .meta
- json file that include options for manipulating related data in .json file.
- tweaked manually in editor (or from game thru network messages in editor).

# .json (lossless)
- lossless human-readable data file, generated from data file (image, video, mesh, anim, etc...)
- converted by modules

# .cook (lossy)
1. transform data stream as designed { -> deflate, -> ogg, -> jpg -> etc1, -> mesh quantization, etc... } 
1. write binary data (.mp/msgpack or .ion by default)
1. converted by cooker tool

# workload
- consume input file(s) & arguments and output json, and/or meta, and/or cook file(s)

# module
- binary program that does workload, then exits.
- source program that gets compiled with engine, then does workload, then exits.
- script program (lua) + luajit + bunch of dlls { physfs, sdl, assimp, freeimage, etc... } that do workload, then exits.

# resave
1. del **.json
1. restart

# upgrade
1. touch all .meta
1. resave

# import
1. for each file that has no .json : compile file > file.json (module)
1. for each json that has no .meta : options json > file.meta (module)

# cook
1. import
1. for each json+meta pair : cook json+meta > file.cook

# pack
1. cook
1. for each cook file : zip cook >> package.pak (deterministic zip, etc)

# fuse
1. pack
1. for each pak file : cat pak >> game.exe

# cooker (background daemon)
1. monitor data folders
1. import
1. cook
1. broadcast file changes
1. sleep
1. restart

# notes: pipeline

basically,

## pipeline (C and/or lua)

importer = file2json
exporter = json2file

cooker = json(+meta)2asset

## runtime (C)

bgloader = asset2c_object, that can be consumed directly or feed to another system
fgloader = bgloader, then sync

cook-on-the-fly = game asks remote cooker to do json(+meta)2asset conversion (RPC)

# notes: cook description (lossless <-> lossy)

1. foreach file extension (ext), find extc sh/dll/binary/lua compiler
   - (ie, .lua -> luac, .c -> cc, .glsl -> glslc, .obj -> objc, .jpg -> jpgc, etc...)
2. prepend toolchain if possible,
   - (ie, ps4+lua -> ps4-luac, xb1+dds->xb1-ddsc, pc+jpg->pc-jpgc, ...)
3. convert it to json by invoking:
     - `toolchain-extc [options] < /in/input.bin > /out/output.json`, or
     - `toolchain-extc [options] /in/input.bin /out/output.json`
     - (options can be also in environment vars)
4. report warnings and errors.
