program Hello;
var a:array[1..10] of array[10..20] of integer;
begin
    a[1][14]:=2;
    a[2][12]:=3;
    a[10][20]:=a[1][14]+a[2][12];
    write(a[10][20]);
end.
