program Hello;
var i,j,k:integer;
begin
    read(i);
    read(j);
    if i<=j then
        begin
            k:=j;
        end
    else k:=i;
    write(k);
end.