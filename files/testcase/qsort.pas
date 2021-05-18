program quicksort(input,output);
var a:array[0..200000] of int64;
var n:int64;i:longint;

procedure swap(var i,j:int64);
var t:int64;
begin
    t:=i;
    i:=j;
    j:=t;
end;

procedure qsort(l,r:int64);
var i,j,key:int64;
var m:int64;
begin
    if l>=r then exit();
    i:=l;
    j:=r;
    m:=(l+r) div 2;
    key:=a[m];
    swap(a[m],a[l]);
    while i<>j do
    begin
        while (a[j]>=key) and (i<j) do j:=j-1;
        while (a[i]<=key) and (i<j) do i:=i+1;
        swap(a[i],a[j]);
    end;
    swap(a[l],a[i]);
    while (l < i) and (a[i] = a[i-1]) do i := i - 1;
    while (j < r) and (a[j] = a[j+1]) do j := j + 1;
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
        if (i <> n) then write(' ');
    end;
end.