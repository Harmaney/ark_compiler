To build the compiler app, `xmake` is needed. After installing `xmake`, just run `xmake` in the root directory.

To build the visualizer app, `node` and `npm` is needed. Run `npm i` in the `visualizer` directory to install dependencies, and use `npm run build` to generate release files.

To build the server app, just compile `server.cpp` with `g++`. You may need to link pthead library with `-lpthread`.

To use the visualizer, you need to run the server app, and use `npm run serve` in the `visualizer` directory to open the visulizer in browser. Remember to change the address of backend server to yours in `index.js`. 

Ugly code. Strongly suggest that you **DO NOT READ** this.