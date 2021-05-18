program qpow;
var b,p,k:int64;
function qpow(a,b,k:int64):int64;
var res,Pow:int64;
begin
    res:=1;
    Pow:=a;
    while b<>0 do
    begin
        if b mod 2 = 1 then res:=res * Pow mod k;
        Pow:=Pow * Pow mod k;
        b:=b div 2;
    end;
    qpow:=res mod k;
end;
begin
    read(b,p,k);
    write(b);
    write('^');
    write(p);
    write(' mod ');
    write(k);
    write('=');
    write(qpow(b,p,k));
end.