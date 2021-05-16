//global_define
int t0_0=0;
string t0_1 = "arr1";
int t0_2=2;
int t0_3=2;
//pre_struct
struct SP;
//pre_array
typedef int t2_0[10];
typedef SP t1_0[10];
//struct
struct SP{
t2_0 arr1;
};
//main
int a;
{
t1_0 sp;
SP* t1_1;
t1_1=&(sp)[(t0_0)];
t2_0* t1_2;
t1_2=&((*t1_1).arr1);
int* t1_3;
t1_3=&(*t1_2)[(t0_2)];
(*t1_3)=(t0_3);
}
