program Hello;
function magic(k:integer):integer;
begin
    magic:=0;
    if k<0 then
    begin
        exit();
    end;
    magic:=k+magic(k-1);
end;
begin
    write(magic(5));
    
end.
