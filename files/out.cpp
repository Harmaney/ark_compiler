//global_define
string t0_0 = "a";
int t0_1=1;
//pre_struct
struct st;
//struct
struct st{
int a;
};
//0
{
st a;
int* t1_0;
t1_0=&((a).a);
(*t1_0)=(t0_1);
}
