program backpack;
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
    writeln(f[c]);
end.