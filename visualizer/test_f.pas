program SegmentTree;
var a,sum,add:array[1..400020] of int64;
var n,m,x,y,z,opt:int64;
var i:longint;

procedure color(l,r,rt,z:int64);
begin
    add[rt]+=z;
    sum[rt]+=z*(r-l+1);
end;

procedure push_col(l,r,rt:int64);
var m:int64;
begin
    if add[rt]<>0 then
    begin
        m:=(l+r) div 2;
        color(l,m,rt*2,add[rt]);
        color(m+1,r,rt*2+1,add[rt]);
        add[rt]:=0;
    end;
end;
procedure modify(l,r,rt,nowl,nowr,z:int64);
var m:int64;
begin
    if (nowl<=l) and (nowr>=r) then
    begin
        color(l,r,rt,z);
        exit;
    end;
    push_col(l,r,rt);
    m:=(l+r) div 2;
    if nowl<=m then
    begin
        modify(l,m,rt*2,nowl,nowr,z);
    end;
    if nowr>m then
    begin
        modify(m+1,r,rt*2+1,nowl,nowr,z);
    end;
    sum[rt]:=sum[rt*2]+sum[rt*2+1];
    exit;
end;

function query(l,r,rt,nowl,nowr:int64):int64;
var m,ans:int64;
begin
    {writeln(l,r,rt);}
    if (nowl<=l) and (nowr>=r) then
    begin
        query:=sum[rt];
        exit;
    end;
    push_col(l,r,rt);
    m:=(l+r) div 2;
    ans:=0;
    if nowl<=m then
    begin
        ans+=query(l,m,rt*2,nowl,nowr);
    end;
    if(nowr>m) then
    begin
        ans+=query(m+1,r,rt*2+1,nowl,nowr);
    end;
    query:=ans;
end;

procedure build(l,r,rt:int64);
var m:int64;
begin
    add[rt]:=0;
    if l=r then
    begin
        sum[rt]:=a[l];
        exit;
    end;
    m:=(l+r) div 2;
    build(l,m,rt*2);
    build(m+1,r,rt*2+1);
    sum[rt]:=sum[rt*2]+sum[rt*2+1];
end;

begin
    Read(n);
    Read(m);
    for i:=1 to n do
    begin
        Read(a[i]);
    end;
    build(1,n,1);
    for i:=1 to m do
    begin
        Read(opt);
        if opt=1 then
        begin
            Read(x);
            Read(y);
            Read(z);
            modify(1,n,1,x,y,z);
        end;
        if opt=2 then
        begin
            Read(x);
            Read(y);
            writeln(query(1,n,1,x,y));
        end;
    end;
end.
{{https://leftvalues.github.io/results.html?a=38.2&b=82.8&c=66.7&d=30.4&e=50.0&f=29.2&g=27.9}}