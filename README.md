To build compiler app, `xmake` is needed. After installing `xmake`, just run `xmake` in the root directory.

To build visualizer app, `node` and `npm` is needed. Run `npm i` in the `visualizer` directory to install dependencies, and use `npm run build` to generate release files.

To build server app, just compile `server.cpp` with `g++`. You may need to link pthead library with `lpthread`.

Ugly code. Strongly suggest that you **DO NOT READ** this.