program Hello;
var i:record
    name:string;
    age:integer;
end;
begin
    i.name:='123';
    i.age:=2;
    write(i.name,i.age);
end.
