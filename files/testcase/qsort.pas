program quicksort(input,output);
var a:array[0..1000] of integer;
var n,i:integer;
procedure swap(var i,j:integer);
var t:integer;
begin
    t:=i;
    i:=j;
    j:=t;
end;
procedure qsort(l,r:integer);
var m:integer;
var i,j,key:integer;
begin
    if l>=r then exit();
    m:=(l+r) div 2;
    i:=l;
    j:=r;
    key:=a[l];
    while i<>j do
    begin
        while (a[j]>=key) and (i<j) do j:=j-1;
        while (a[i]<=key) and (i<j) do i:=i+1;
        swap(a[i],a[j]);
    end;
    swap(a[l],a[i]);
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
    end;
end.