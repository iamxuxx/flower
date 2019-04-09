

#include "math.h"
#include "windows.h"
#include "okapi32.h"

#define HEIGHT 720
#define WIDTH 2560
#define H2 HEIGHT/2
#define W2 WIDTH/2
#define HW HEIGHT*WIDTH
#define HW2 H2*W2


#define H4 H2/2
#define W4 W2/2
#define W8 W4/2

#define NEAR_FAR 40. //毫米
#define RADIUS 140 //毫米
#define BCH RADIUS*1.4142


#define pi 3.1415926535898
#define II 15
#define JJ 19
#define IJ II*JJ
#define COUNTLIMIT 2000
#define II2 17		//网格点的行
#define JJ2 21		//网格点的列
#define IJ2 II2*JJ2


struct FXY{float x,y;};
struct FXYZ{float x,y,z;};
struct BILN {int no;FXY xy;FXY uv;};
struct BILN4 {FXY xy[4];FXY uv[4];};
struct PNT{BYTE b,g,r;};
struct FABCD{float A,B,C,D;};
struct RCTLOG{
	float i,j,r;
	float CA,CB,CG;
	FXYZ dot;
	FXYZ DOT;
};
struct FANGB{
	RCTLOG rl[5];	//除i,j外，均是修正值
	FABCD s;		//靶平面方程像素单位
	FABCD S;		//靶平面方程毫米单位
	float er;		//第二点误差像素单位
	float eg;		//平均边长像素单位
	float EG;		//平均边长差像素单位
	float amp;		//第2点的牺牲误差
	float id;		//第2点的牺牲误差
	float Rt[12];
	float RT[12];
	RECT rect;
};
struct LINE{FXYZ d0,d1;float A,C;};//d0,d1=两点式；A,C=斜率截距式
//struct LINE{FXYZ d0,d1;};
struct BLOCK {
	FXY a[4],	//四边形顺序节点
		fi;		//四边形内一质点
	FXYZ L[4];	//四个方程式:L0:a0-a1;  L1:a1-a2;  L2:a2-a3;  L3:a3-a0
	float d[4],//四条边长:  d0:a0-a1;  d1:a1-a2;  d2:a2-a3;  d3:a3-a0
		s,		//
		t;		//
};
struct CROS {
	short b1,e1,b2,e2;
};
struct IDX{short no[4];};


//=========================================================



int all_gauss( int n, float *a, float *b);
float area(FXY *b);//三角形面积
int BWLabel(int *INput,int row,int col);
void cld2eig(FXYZ *f,int n,float *Q,float *A);
float cosalf(FXYZ e1,FXYZ e3);
void Corrosion(BYTE *image_in, BYTE *image_out, int xsize, int ysize,int d);         //腐蚀算法
void dda(POINT b,POINT e,int hght,int wdth,BYTE *byt,BYTE gray);//dda 算法
void ddaC(POINT b,POINT e,int hght,int wdth,PNT *pnt,PNT gray);//ddaC 算法
void Dilation(BYTE *image_in, BYTE *image_out, int xsize, int ysize);         //膨胀算法
float dot2line(FXY fi,FXYZ L);//点到直线的距离
void Entity(float *R);		//生成单位阵R
void ezh(BYTE *bf,BYTE thrd);
float findID(float id,float t,float ed,RCTLOG rl0[5],RCTLOG rl1[5],FXY pt[8]);
float findx(float a,float b,float delt,RCTLOG rl[5],float id);
float fx(float r,RCTLOG rl[5],float id); 
void GnrtIDX(IDX* idx,int n);	//生成网格CELL编组
void GetColor(BYTE *yuv,PNT *pnt,int height,int width);//从yuv取彩色图
void GetGray(BYTE *yuv,BYTE *buf,int height,int width);//从yuv取灰度图
void GetR3(float *R3,float *R4);
void GetR4(float *R3,float *R4);
FXYZ guiy(FXYZ v);
void histogram (BYTE *Gray_in,int *histogramData,int row,int col);
void im2bw(BYTE *gray_matrix,BYTE *binary_image,int row,int col);  //将图像二值化
float im2bw2(BYTE *gray_matrix,BYTE *binary_image,int row,int col,float pcnt);
//template <class Any>void InitiateMatrix(Any **matrix,int row,int col){for (int i=0;i<row;++i){matrix[i]=new Any[col];for (int j=0;j<col;++j)matrix[i][j]=0;}}
bool InLine(FXY c,FXY d,FXY f1);//点f1在直线cd上?
bool InQd(FXY a[4],FXY f1);	//质点是否在四边形内
FXYZ intsct(FXYZ pwk,FABCD s0);	//畸变校正后的屏幕点与靶标平面方程式交点
void inv3(float *R3);
void inv4(float *R4);
int jcbi(float *X,int n,float *R,float eps,int jt);
void LftTop(FXY *f0,long n,FXY *fj);//取最附近4点送fj
FXYZ Leg(FXYZ L0,FXYZ L1);//二元一次方程组、两平面直线相交
FXYZ Line(FXY p0,FXY p1);	//返回平面直线方程 A*x+B*y+C=0
void ltong(BYTE *bf,FXY *jd);//连通区靶点
float makeID(RCTLOG rl[4],float EDGE,float id);
float makeOld(FANGB *fb,float Edge,float id);
void mmtrx(float *R0,float *R1,float *R2);//方阵相乘 R2=R0*R1
void mmtrx4(float *R0,float *R1,float *R2);//方阵相乘 R2=R0*R1
void mmvct(float *R,float *v);//方阵相乘 R2=R0*R1
FXYZ mvct4a(float *Q,FXYZ v);//方阵乘向量 v2=Q*v1
FXYZ normal(FXYZ v0,FXYZ v1);
void o2R(FXYZ ro,float *R);//欧拉角变换成R矩阵
float p2S(FXYZ p,FABCD S); //点p到平面S的距离
FXYZ pRtq(float *Rt,FXYZ q);	//p=t+RT*q
FXYZ qRtp(float *Rt,FXYZ p);	//q=RT*(p-t)
FXYZ R2o(float *R);//R矩阵中分离出欧拉角
FABCD R2S(float *R);//R矩阵中分离出S平面（{0,0,z}截面）
BLOCK s_t(BLOCK bok);
FXYZ sbstrct(FXYZ f0,FXYZ f1);
float skew(LINE ln0,LINE ln1);
FXYZ skew3(LINE ln0,LINE ln1);
void sgma(int c,int r,float *f0,float *sg);//均方差 结果村sg[] 参考用
//void Sortbno(BILN arr[],long cnt);
void Sort7x(FXYZ arr[],long cnt);
void Sort7y(FXYZ arr[],long cnt);
void TriLn(FXYZ *f,long n,FXYZ *g);//3D直线拟合二点式
void trnpos(float *R0);
void Uv2Xy(BYTE *byh,BILN4 *bl4,float *Qa,float id);//uv2xy
FXY xy2uv(FXY *fj,FXY *jdz,BILN4 *bl4,float *Q,float id);//xy2uv
FXYZ ymxj(LINE ln0,LINE ln1,float *d);//异面相交 input:两条空间直线(两点式)；output:FXYZ c(交点)，float d(异面直线距离)
void YUVGetColr(BYTE *yuv,PNT *pnt,int height,int width);
void YUVGetBW(BYTE *yuv,BYTE *buf,int height,int width);
void ZRtt(double *R,double alf);//取绕z轴转alf的R
