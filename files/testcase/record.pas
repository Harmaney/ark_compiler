program Hello;
var i:record
    name:string;
    age:int64;
end;
begin
    i.name:='123';
    i.age:=2;
    writeln(i.name,i.age);
end.
