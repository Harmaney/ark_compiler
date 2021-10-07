
import Konva from 'konva';
import 'bootstrap/dist/css/bootstrap.min.css';
import CodeMirror from 'codemirror'
import 'codemirror/lib/codemirror.css'

import info from './info.json';
// let info;

let speedRatio = 1;

let standard = new Konva.Text({
  x: 0,
  y: 0,
  text: '1',
  fontSize: 20,
  fontFamily: 'Consolas',
  fill: 'rgb(0,0,0)',
  padding: 0,
  align: 'left',
});

const [CHR_WIDTH, CHR_HEIGHT] = [standard.width(), standard.height()]
const WORD_LR_PAD = 5, WORD_HEIGHT = -6, CODE_PAD = 10;

// first we need to create a stage
var stage, layer;

// then create layer
var layer = new Konva.Layer();



function Perform(q) {
  q.reduce(
    (prev, next) => prev.then(() => next()),
    Promise.resolve()
  );
}


function PromiseFn(fn) {
  return () => new Promise((res) => {
    fn();
    res();
  });
}

function PromiseAnimation(KeyFrame, duration) {
  duration *= speedRatio;
  return () => new Promise((res) => {
    let anim = new Konva.Animation(function (frame) {
      let proc = Math.min(1, frame.time / duration);
      KeyFrame(proc);
      if (proc >= 1) {
        anim.stop();
        res();
      }
    }, layer);
    anim.start();
  })
}

function PromiseSleep(duration) {
  return () => new Promise(res => setTimeout(res, duration * speedRatio));
}


let floated = [];

class FloatWord {
  constructor(text, fpos, tpos, tcolor, crsp, no, overflowHandler) {
    [this.text, this.fpos, this.tpos, this.tcolor, this.crsp, this.no, this.overflowHandler] = [text, fpos, tpos, tcolor, crsp, no, overflowHandler];

    this.word = new Konva.Text({
      x: fpos.x,
      y: fpos.y,
      text: text,
      fontSize: 20,
      fontFamily: 'Consolas',
      fill: 'rgb(0,0,0)',
      padding: 0,
      align: 'left',
    });

    this.rect = new Konva.Rect({
      x: fpos.x - WORD_LR_PAD,
      y: fpos.y + WORD_HEIGHT,
      width: 0,
      height: 30,
      fill: 'rgb(209,217,224)',
      stroke: 'black',
      strokeWidth: 0,
      cornerRadius: 5
    });

  }

  AnimationSequence() {
    let seq = [() => new Promise((res) => {
      layer.add(this.rect);
      layer.add(this.word);
      floated.push(this.word);
      res();
    })];
    // console.log(this.text.length);

    for (let i = 0; i < this.text.length; ++i) {
      seq.push(
        PromiseAnimation((proc) => {
          this.rect.width(WORD_LR_PAD
            + i * CHR_WIDTH
            + proc * (CHR_WIDTH + (i == this.text.length - 1 ? WORD_LR_PAD : 0)));
        }, 50),
        PromiseSleep(50)
      );
    }

    seq.push(PromiseAnimation((proc) => {
      proc = -proc * proc + 2 * proc;
      this.word.fill(`rgb(${this.tcolor.r * proc},${this.tcolor.g * proc},${this.tcolor.b * proc})`)
    }, 200),
      PromiseFn(() => {
        this.crsp.fill(`rgb(${this.tcolor.r},${this.tcolor.g},${this.tcolor.b})`);
      }),
      () => new Promise((res) => {
        let fy = new Array(floated.length);
        for (let i in floated)
          fy[i] = floated[i].y();
        let move = (fy[fy.length - 2] == this.tpos.y);
        // console.log(fy[fy.length - 2], this.tpos.y);

        let obj = this;
        let anim = new Konva.Animation(function (frame) {
          let duration = 500 * speedRatio, proc = Math.min(1, frame.time / duration);
          proc = -proc * proc + 2 * proc;
          if (move)
            for (let i in floated)
              floated[i].y(fy[i] - CHR_HEIGHT * proc);
          obj.word.x(obj.fpos.x + proc * (obj.tpos.x - obj.fpos.x));
          obj.word.y(obj.fpos.y + proc * (obj.tpos.y - obj.fpos.y));
          obj.rect.x(obj.fpos.x - WORD_LR_PAD + proc * (obj.tpos.x - obj.fpos.x));
          obj.rect.y(obj.fpos.y - WORD_LR_PAD + proc * (obj.tpos.y - obj.fpos.y));
          obj.rect.opacity(1 - proc);
          if (proc >= 1) {
            anim.stop();
            res();
          }
        }, layer);
        anim.start();
      })
    );
    return seq;
  }
};

const typeColor = {
  'default': { r: 0, g: 0, b: 0 },
  'red': { r: 215, g: 58, b: 73 },
  'purple': { r: 111, g: 66, b: 193 },
  'gray': { r: 106, g: 115, b: 125 },
  'blue': { r: 0, g: 92, b: 197 }
};

function GetType(id) {
  if (id == 'keyword')
    return 'red';
  if (id == 'int' || id == 'float')
    return 'blue';
  return 'default';
}

let floatWords = [], lexOperations = []

import cytoscape from 'cytoscape';

class TreeGraph {
  constructor(elemID) {
    this.elemID = elemID;
    this.nodes = [];
    this.edges = [];
    this.labelOf = {};
    this.allNode = new Set();
  }
  UpdateView(callback, highlight) {
    let { nodes, edges } = this;
    if (!highlight)
      highlight = nodes.length - 1;
    else {
      for (let i = 0; i < nodes.length; ++i)
        if (highlight == nodes[i].data.id) {
          highlight = i;
          break;
        }
    }
    console.log(highlight);
    for (let n of nodes)
      n.data.active = "";
    nodes[highlight].data.active = "active";


    let [prePan, preZoom] = this.cy ? [this.cy.pan(), this.cy.zoom()] : [-1, -1];

    this.cy = cytoscape({
      container: document.getElementById(this.elemID),
      zoomingEnabled: true,
      panningEnabled: true,
      wheelSensitivity: 0.1,

      layout: {
        name: 'preset'
      },

      style: [
        {
          selector: 'node',
          css: {
            'shape': 'square',
            'label': 'data(label)',
            'text-valign': 'center',
            'text-halign': 'center',
            'height': '30px',
            'background-opacity': '0',
            'font-family': 'Consolas',
          }
        },
        {
          selector: `node[active="active"]`,
          css: {
            'background-color': 'red',
            'background-opacity': '1'
          }
        },

        {
          selector: 'edge',
          css: {
            'width': 2,
            'line-color': 'gray',
            'curve-style': 'bezier'
          }
        }
      ],

      elements: {
        nodes: nodes,
        edges: edges
      }
    });
    this.cy.nodes(`[active="active"]`).animate({ 'style': { 'background-opacity': '0' } }, { 'duration': 600 });
    // console.log(nodes[highlight].position);
    this.cy.ready(
      () => {
        /*cy.pan({
          x: 100 - nodes[highlight].position.x,
          y: 100 - nodes[highlight].position.y
        });*/
        // console.log(cy.pan());
        if (preZoom != -1) {
          this.cy.zoom(preZoom);
          this.cy.pan(prePan);
        }
        callback();
      }
    )
  }

  Add(node) {
    let { nodes, edges, labelOf, allNode } = this, hasIn = new Set(), position = {};

    for (let n of nodes)
      n.data.active = "";
    allNode.add(node.ID);
    const CHR_WIDTH = 9;
    labelOf[node.ID] = node.label;
    // console.log(node);
    nodes.push({
      data: {
        id: node.ID,
        label: labelOf[node.ID],
        active: "",
      },
      css: {
        width: `${labelOf[node.ID].length * CHR_WIDTH}`
      }
    })

    if (node.son) {
      // console.log(node)
      for (let s of node.son) {
        edges.push({ data: { source: node.ID, target: s } })
      }
    }
    let edgeOut = {}
    for (let e of edges) {
      let [x, y] = [e.data.source, e.data.target]
      if (!edgeOut[x])
        edgeOut[x] = []
      edgeOut[x].push(y)
      hasIn.add(parseInt(y));
    }

    let left = {}, right = {}
    // console.log(nodes);
    function DFS(n) {
      // console.log(n, labelOf[n]);
      left[n] = -labelOf[n].length * CHR_WIDTH / 2;
      right[n] = labelOf[n].length * CHR_WIDTH / 2;
      if (edgeOut[n])
        for (let i = 0; i < edgeOut[n].length; ++i) {
          DFS(edgeOut[n][i])
          if (i == 0) {
            left[n] = Math.min(left[n], left[edgeOut[n][i]])
            right[n] = Math.max(right[n], right[edgeOut[n][i]])
          } else {
            right[n] += -left[edgeOut[n][i]] + right[edgeOut[n][i]]
          }
        }
    };

    for (let n of allNode)
      if (!hasIn.has(n)) {
        DFS(n)
      }

    const HEIGHT = 100

    function SetIndent(n, lev, indent) {
      position[n] = { x: indent, y: lev * HEIGHT }
      if (edgeOut[n])
        for (let i = 0; i < edgeOut[n].length; ++i) {
          if (i)
            indent -= left[edgeOut[n][i]];
          SetIndent(edgeOut[n][i], lev + 1, indent)
          indent += right[edgeOut[n][i]];
        }
    };

    let acc = 0;

    for (let n of allNode)
      if (!hasIn.has(n)) {
        acc -= left[n]
        SetIndent(n, 0, acc)
        acc += right[n]
      }

    for (let n of nodes)
      n.position = position[n.data.id]
  }
};

let pst = new TreeGraph("pst"), ast = new TreeGraph("ast");

let buildOperations = [], genOperations = [];

function SyncToken() {
  let found = false;
  for (let e of document.getElementById("stream").childNodes) {
    if (e.getBoundingClientRect().right >= document.getElementById("dash").getBoundingClientRect().right && !found) {
      found = true;
      e.classList.add("lex-token-active");
    } else
      e.classList.remove("lex-token-active");
  }
}


let dragging = false;
document.onmousemove = (e) => {
  if (dragging) {
    document.getElementById("stream").scrollBy(-e.movementX, 0);
    SyncToken();
  }
}

document.getElementById("stream").onmousedown = (e) => {
  dragging = true;
}

document.onmouseup = (e) => {
  dragging = false;
}


function Animations() {
  let tokens = '';
  for (let e of info.lex) {
    let c = typeColor[GetType(e.type)];
    tokens += `<span class="lex-token" style="display:block"><code style="color:rgb(${c.r},${c.g},${c.b}) ">${e.word}</code></span>`;
  }
  document.getElementById("stream").innerHTML = tokens;

  SyncToken();

  for (let n of info.parser) {
    n.label = n.type == "grammar" ? (n.raw == "" ? n.parserSymbol : n.raw) : n.type;

    let goal = (n.type == "grammar" ? pst : ast);
    buildOperations.push(() => new Promise((res) => {
      // console.log(n);
      goal.Add(n);
      goal.UpdateView(res);
    }));

    buildOperations.push(PromiseSleep(1000));
  }

  for (let g of info.gen) {
    if (g.ARRIVE) {
      // console.log(g.ARRIVE);
      genOperations.push(() => new Promise((res) => {
        ast.UpdateView(res, g.ARRIVE);
      }));
    } else {
      genOperations.push(PromiseFn(() => {
        let block = document.getElementById(g.GEN.TO);
        block.innerHTML = block.innerHTML + g.GEN.CODE + '\n';
      }));
    }
    genOperations.push(PromiseSleep(1000));
  }
}

import $ from "jquery";

let cm = CodeMirror.fromTextArea(document.getElementById("pas-code"), {
  lineNumbers: true, // 显示行号
})
cm.setSize("100%", "100%")


const SEC = ["prelude", "global_define", "pre_struct",
  "pre_array", "struct", "init_global_var",
  "main"];
let code_head = "", code_pre = "";
for (let code_section of SEC) {
  code_head += `<td ${code_section == "main" ? `style="width:400px"` : ""}>${code_section} </td>`;
  code_pre += `<td><pre id = "${code_section}" ></pre></td>`
}
// document.getElementById("ccodehd").innerHTML = code_head;
//document.getElementById("ccode").innerHTML = code_pre;

document.getElementById("compile").onclick = () => {
  console.log(cm.getValue());
  $.ajax({
    method: 'POST', dataType: "json",
    url: 'http://localhost:8848/compile',
    data: { code: cm.getValue() },
    success: (resp) => {
      info = resp;
      console.log(resp);
      Work();
    },
  });
}

function Work() {
  document.getElementById("editor").style.display = "none";
  document.getElementById("animation").style.display = "block";
  Animations();
}

Work();

/*
document.getElementById("do-lex").onclick = () => {
  Perform(lexOperations);
  document.getElementById("do-build").disabled = false;
  document.getElementById("do-lex").disabled = true;
}

document.getElementById("do-build").onclick = () => {
  Perform(buildOperations);
  document.getElementById("do-build").disabled = true;
  document.getElementById("do-generate").disabled = false;
}

document.getElementById("do-generate").onclick = () => {
  Perform(genOperations);
  document.getElementById("do-generate").disabled = true;
}

document.getElementById("speedbar").oninput = function () {
  speedRatio = Math.pow(100, (0.5 - this.value / (this.max - this.min)));
  if (this.value == this.max)
    speedRatio = 0.00001;
  else if (this.value == this.min)
    speedRatio = 10;
}
*/