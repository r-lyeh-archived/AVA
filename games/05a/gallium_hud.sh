#!/bin/bash
cd "$(dirname -- "$(readlink -fn -- "${0}")")"

GALLIUM_HUD="fps;draw-calls;requested-VRAM+requested-GTT;vs-invocations+ps-invocations+num-compilations" ./bin/app
