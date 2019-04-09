

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

#define NEAR_FAR 40. //����
#define RADIUS 140 //����
#define BCH RADIUS*1.4142


#define pi 3.1415926535898
#define II 15
#define JJ 19
#define IJ II*JJ
#define COUNTLIMIT 2000
#define II2 17		//��������
#define JJ2 21		//��������
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
	RCTLOG rl[5];	//��i,j�⣬��������ֵ
	FABCD s;		//��ƽ�淽�����ص�λ
	FABCD S;		//��ƽ�淽�̺��׵�λ
	float er;		//�ڶ���������ص�λ
	float eg;		//ƽ���߳����ص�λ
	float EG;		//ƽ���߳������ص�λ
	float amp;		//��2����������
	float id;		//��2����������
	float Rt[12];
	float RT[12];
	RECT rect;
};
struct LINE{FXYZ d0,d1;float A,C;};//d0,d1=����ʽ��A,C=б�ʽؾ�ʽ
//struct LINE{FXYZ d0,d1;};
struct BLOCK {
	FXY a[4],	//�ı���˳��ڵ�
		fi;		//�ı�����һ�ʵ�
	FXYZ L[4];	//�ĸ�����ʽ:L0:a0-a1;  L1:a1-a2;  L2:a2-a3;  L3:a3-a0
	float d[4],//�����߳�:  d0:a0-a1;  d1:a1-a2;  d2:a2-a3;  d3:a3-a0
		s,		//
		t;		//
};
struct CROS {
	short b1,e1,b2,e2;
};
struct IDX{short no[4];};


//=========================================================



int all_gauss( int n, float *a, float *b);
float area(FXY *b);//���������
int BWLabel(int *INput,int row,int col);
void cld2eig(FXYZ *f,int n,float *Q,float *A);
float cosalf(FXYZ e1,FXYZ e3);
void Corrosion(BYTE *image_in, BYTE *image_out, int xsize, int ysize,int d);         //��ʴ�㷨
void dda(POINT b,POINT e,int hght,int wdth,BYTE *byt,BYTE gray);//dda �㷨
void ddaC(POINT b,POINT e,int hght,int wdth,PNT *pnt,PNT gray);//ddaC �㷨
void Dilation(BYTE *image_in, BYTE *image_out, int xsize, int ysize);         //�����㷨
float dot2line(FXY fi,FXYZ L);//�㵽ֱ�ߵľ���
void Entity(float *R);		//���ɵ�λ��R
void ezh(BYTE *bf,BYTE thrd);
float findID(float id,float t,float ed,RCTLOG rl0[5],RCTLOG rl1[5],FXY pt[8]);
float findx(float a,float b,float delt,RCTLOG rl[5],float id);
float fx(float r,RCTLOG rl[5],float id); 
void GnrtIDX(IDX* idx,int n);	//��������CELL����
void GetColor(BYTE *yuv,PNT *pnt,int height,int width);//��yuvȡ��ɫͼ
void GetGray(BYTE *yuv,BYTE *buf,int height,int width);//��yuvȡ�Ҷ�ͼ
void GetR3(float *R3,float *R4);
void GetR4(float *R3,float *R4);
FXYZ guiy(FXYZ v);
void histogram (BYTE *Gray_in,int *histogramData,int row,int col);
void im2bw(BYTE *gray_matrix,BYTE *binary_image,int row,int col);  //��ͼ���ֵ��
float im2bw2(BYTE *gray_matrix,BYTE *binary_image,int row,int col,float pcnt);
//template <class Any>void InitiateMatrix(Any **matrix,int row,int col){for (int i=0;i<row;++i){matrix[i]=new Any[col];for (int j=0;j<col;++j)matrix[i][j]=0;}}
bool InLine(FXY c,FXY d,FXY f1);//��f1��ֱ��cd��?
bool InQd(FXY a[4],FXY f1);	//�ʵ��Ƿ����ı�����
FXYZ intsct(FXYZ pwk,FABCD s0);	//����У�������Ļ����б�ƽ�淽��ʽ����
void inv3(float *R3);
void inv4(float *R4);
int jcbi(float *X,int n,float *R,float eps,int jt);
void LftTop(FXY *f0,long n,FXY *fj);//ȡ���4����fj
FXYZ Leg(FXYZ L0,FXYZ L1);//��Ԫһ�η����顢��ƽ��ֱ���ཻ
FXYZ Line(FXY p0,FXY p1);	//����ƽ��ֱ�߷��� A*x+B*y+C=0
void ltong(BYTE *bf,FXY *jd);//��ͨ���е�
float makeID(RCTLOG rl[4],float EDGE,float id);
float makeOld(FANGB *fb,float Edge,float id);
void mmtrx(float *R0,float *R1,float *R2);//������� R2=R0*R1
void mmtrx4(float *R0,float *R1,float *R2);//������� R2=R0*R1
void mmvct(float *R,float *v);//������� R2=R0*R1
FXYZ mvct4a(float *Q,FXYZ v);//��������� v2=Q*v1
FXYZ normal(FXYZ v0,FXYZ v1);
void o2R(FXYZ ro,float *R);//ŷ���Ǳ任��R����
float p2S(FXYZ p,FABCD S); //��p��ƽ��S�ľ���
FXYZ pRtq(float *Rt,FXYZ q);	//p=t+RT*q
FXYZ qRtp(float *Rt,FXYZ p);	//q=RT*(p-t)
FXYZ R2o(float *R);//R�����з����ŷ����
FABCD R2S(float *R);//R�����з����Sƽ�棨{0,0,z}���棩
BLOCK s_t(BLOCK bok);
FXYZ sbstrct(FXYZ f0,FXYZ f1);
float skew(LINE ln0,LINE ln1);
FXYZ skew3(LINE ln0,LINE ln1);
void sgma(int c,int r,float *f0,float *sg);//������ �����sg[] �ο���
//void Sortbno(BILN arr[],long cnt);
void Sort7x(FXYZ arr[],long cnt);
void Sort7y(FXYZ arr[],long cnt);
void TriLn(FXYZ *f,long n,FXYZ *g);//3Dֱ����϶���ʽ
void trnpos(float *R0);
void Uv2Xy(BYTE *byh,BILN4 *bl4,float *Qa,float id);//uv2xy
FXY xy2uv(FXY *fj,FXY *jdz,BILN4 *bl4,float *Q,float id);//xy2uv
FXYZ ymxj(LINE ln0,LINE ln1,float *d);//�����ཻ input:�����ռ�ֱ��(����ʽ)��output:FXYZ c(����)��float d(����ֱ�߾���)
void YUVGetColr(BYTE *yuv,PNT *pnt,int height,int width);
void YUVGetBW(BYTE *yuv,BYTE *buf,int height,int width);
void ZRtt(double *R,double alf);//ȡ��z��תalf��R
