
import Konva from 'konva';
import 'bootstrap/dist/css/bootstrap.min.css';
import CodeMirror from 'codemirror'
import 'codemirror/lib/codemirror.css'

import './style.css';

//import info from './info.json';
let info;
import grammarText from './grammar.json';

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
    this.Init();
  }

  Init() {
    this.nodes = [];
    this.edges = [];
    this.labelOf = {};
    this.allNode = new Set();
  }

  UpdateView(highlight) {
    let { nodes, edges } = this;

    for (let n of nodes)
      n.data.active = "";
    for (let h of highlight) {
      for (let i = 0; i < nodes.length; ++i)
        if (h == nodes[i].data.id) {
          nodes[i].data.active = "active";
          break;
        }
    }

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
            'font-family': 'CMU Typewriter Text',
          }
        },
        {
          selector: `node[active="active"]`,
          css: {
            'background-color': '#d9d9d9',
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

/*

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
*/

let lastAction = -1, node = {};

function Action(k) {
  let entries = document.getElementById("action").childNodes;
  console.log(k);
  for (let i = 0; i < entries.length; ++i)
    if (i <= k)
      entries[i].classList.add("done");
    else
      entries[i].classList.remove("done");

  let hl = [];
  if (lastAction > k) {
    lastAction = -1;
    pst.Init();
    hl = undefined;
  }
  for (let i = lastAction + 1; i <= k; ++i) {
    if (info.action[i].newnodes)
      for (let n of info.action[i].newnodes) {
        if (node[n].type == "grammar") {
          pst.Add(node[n]);
          if (hl)
            hl.push(node[n].ID);
        }
      }
  }
  pst.UpdateView(hl || []);
  lastAction = k;
}

window.Action = Action;

function Animations() {
  let tokens = '';
  for (let e of info.lex) {
    let c = typeColor[GetType(e.type)];
    tokens += `<span class="lex-token" style="display:block"><code style="color:rgb(${c.r},${c.g},${c.b}) ">${e.word}</code></span>`;
  }
  // document.getElementById("stream").innerHTML = tokens;


  //SyncToken();


  let actions = ``;
  for (let i in info.action) {
    let a = info.action[i];
    let fmtState = '';
    for (let s of a.states)
      fmtState += `${s} `;
    let fmtPrefix = '';
    for (let p of a.prefix)
      fmtPrefix += `${p} `;
    let suf = `<td class="state">${fmtState}</td><td>${fmtPrefix}</td>`;
    if (a.action == 'reduce')
      actions += `<tr class="act" onclick="Action(${i})"><td class="tok">${a.tok[1]}</td><td><b>Reduce</b> with <span class="formula">${grammarText[a.reduce - 1].replace('-> @', '-> ε').replace('->', '→').replace('@', '')}</span></td>${suf}</tr>`;
    else if (a.action == 'shift')
      actions += `<tr class="act" onclick="Action(${i})"><td class="tok">${a.tok[1]}</td><td><b>Shift</b> to <span class="state">${a.states[a.states.length - 1]}</span></td>${suf}</tr>`;
    else
      actions += `<tr class="act" onclick="Action(${i})"><td class="tok">${a.tok[1]}</td><td><b>ACC</b></td>${suf}</tr>`;
  }

  document.getElementById("action").innerHTML = actions;

  for (let n of info.parser) {
    n.label = n.type == "grammar" ? (n.raw == "" ? n.parserSymbol : n.raw) : n.type;
    node[n.ID] = n;
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
  code_head += `< td ${code_section == "main" ? `style="width:400px"` : ""}> ${code_section} </td > `;
  code_pre += `< td > <pre id="${code_section}" ></pre></td > `
}
// document.getElementById("ccodehd").innerHTML = code_head;
//document.getElementById("ccode").innerHTML = code_pre;

document.getElementById("compile").onclick = () => {
  console.log(cm.getValue());
  let done = false;
  setTimeout(() => {
    if (!done) {
      alert("出错了，大概率是编译不通过，小概率是网络问题，如果实在找不到问题的话可以发给我看看，邮箱是mail@yiren.lu。");
      location.reload();
    }
  }, 5000);

  $.ajax({
    method: 'POST', dataType: "json",
    url: 'http://81.70.144.208:8848/compile',
    data: { code: cm.getValue() },
    success: (resp) => {
      info = resp;
      done = true;
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

function SetCode(id) {
  const CODE = [
    `program Hello;
var a,b:integer;
begin
    read(a,b);
    write(a+b);
end.`,
    `program example(input,output);
var x,y:integer;
function gcd(a,b:integer):integer;
    begin 
        if b=0 then gcd:=a
        else gcd:=gcd(b, a mod b)
    end;
begin
    read(x, y);
    write(gcd(x, y))
end.`,
    `program Hello;
var a:array[1..10,10..20] of integer;
begin
    a[1,14]:=2;
    a[2,12]:=3;
    a[10,20]:=a[1,14]+a[2,12];
    write(a[10,20]);
end.`,
    `program backpack;
var c,n:longint;f,w,v:array[0..1005] of longint;
var i,j:longint;
function max(i,j:longint):longint;
begin
    if i<j then max:=j
    else max:=i;
end;
begin
    read(c,n);
    for i:=1 to n do
        read(w[i],v[i]);
    for i:=1 to n do
    begin
        j:=C;
        while j>=w[i] do
        begin
            f[j]:=max(f[j],f[j-w[i]]+v[i]);
            j:=j-1;
        end;
    end;
    write(f[c]);
end.`,
    `program quicksort(input,output);
var a:array[0..200000] of int64;
var n:int64;i:longint;

procedure swap(var i,j:int64);
var t:int64;
begin
    t:=i;
    i:=j;
    j:=t;
end;

procedure qsort(l,r:int64);
var i,j,key:int64;
var m:int64;
begin
    if l>=r then exit();
    i:=l;
    j:=r;
    m:=(l+r) div 2;
    key:=a[m];
    swap(a[m],a[l]);
    while i<>j do
    begin
        while (a[j]>=key) and (i<j) do j:=j-1;
        while (a[i]<=key) and (i<j) do i:=i+1;
        swap(a[i],a[j]);
    end;
    swap(a[l],a[i]);
    while (l < i) and (a[i] = a[i-1]) do i := i - 1;
    while (j < r) and (a[j] = a[j+1]) do j := j + 1;
    qsort(l,i-1);
    qsort(j+1,r);
end;

begin
	read(n);
	for i:=1 to n do
		read(a[i]);
	qsort(1,n);
    for i:=1 to n do
    begin
        write(a[i]);
        if (i <> n) then write(' ');
    end;
    write('\\n');
end.`
  ];

  cm.setValue(CODE[id]);
}

window.SetCode = SetCode;