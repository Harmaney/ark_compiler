program Hello;
var i,j,k:integer;
begin
    read(i);
    read(j);
    k:=i;
    if i<=j then
        begin
            k:=j;
        end;
    write(k);
end.