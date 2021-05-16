program Hello;
var i,j:integer;
function magic(k:int64):int64;
begin
    magic:=j;
    j:=j+1;
    if j<=5 then
    begin
        magic:=magic(magic(k))+magic+k;
    end;
end;
function magic:int64;
begin
    magic:=i;
    i:=i+1;
    if i<=5 then
    begin
        magic:=magic+magic()+magic(magic);
    end;
end;
var y:integer;
begin
    i:=0;
    j:=0;
    writeln(magic);
    
end.
