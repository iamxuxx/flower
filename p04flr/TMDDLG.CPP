// TmdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tmd.h"
#include "TmdDlg.h"
#include "okapi32.h"
#include "sub.h"
#include <io.h>

#include "math.h"
#include <stdio.h>
#include <vector>
#include <istream>
#include <fstream> 
#include <string>
using namespace std;
  #include   <algorithm>
  #include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define THRT 30
#define THR2 THRT*2
#define THR4 THR2*THR2
#define R_LMT 4   //PIXEL
#pragma comment(lib, "OKAPI32")
#pragma comment(lib, "Sub")
//struct SH3 {long k,unsigned short g;float v;};
//struct SH4 {unsigned short i,j,g;float v;};	
//struct SH6 {unsigned short i,j,g1,g2;float v1,v2;};	
//struct RCRD {BYTE no,rng;SH4 s;};
struct ELPS{
	float	a,		//a轴
	  		b,		//b轴
			bt,		//a与x轴夹角
			ag,		//起点角
			me;		//均方差
	FXY		c;		//圆心
};	
struct F2XY{float	x0,y0,x1,y1;};


FXYZ *cld=(FXYZ *)new FXYZ[3000];

HANDLE hBrd;
PNT *pnt,*pnu,BLACK={0,0,0},WHITE={255,255,255},RED={0,0,255};
	BYTE *bg=(BYTE *)new BYTE[HW];
BYTE *b0,*b1,gray;
RECT rct0;
FXYZ dt[6][25];

FXYZ stdd3(float *a,int n) ;
void Sort7z(FXYZ arr[],long cnt,BYTE flg);
void CTgt2Q(float id,FXYZ *dt1,float *Q);
float tsFx(float a,FXYZ t1,FXYZ c0);
float tsFind(float a,float b,FXYZ t1,FXYZ c0);
ELPS fitElp(FXYZ *xy,long n);
void genXX(FXYZ *xy,long n,float *XX);
FXYZ pdt2vct(FXYZ in,float &r);
float mods(FXYZ in){float r;r=sqrt(in.x*in.x+in.y*in.y+in.z*in.z);return r;};
void dsply();
FXYZ guiy1(FXYZ i,float &r){FXYZ f;r=sqrt(i.x*i.x+i.y*i.y+i.z*i.z);f.x=i.x/r;f.y=i.y/r;f.z=i.z/r;return f;}
bool maxLcl(float *gv,long k,float &max);
FXYZ maxFxy(float *gv,long k,float &max);
FXYZ maxNine(float *gv,long k,float *co);

vector <FXYZ> F1;
//vector <FXYZ> F2;
vector <FXY> F0;
vector <F2XY> F2;

/////////////////////////////////////////////////////////////////////////////
// CTmdDlg dialog

CTmdDlg::CTmdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTmdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTmdDlg)
	m_id = 0.0f;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmdDlg)
	DDX_Control(pDX, IDC_COMBO2, m_dot);
	DDX_Control(pDX, IDC_COMBO1, m_cmb);
	DDX_Control(pDX, IDC_LIST1, m_lst);
	DDX_Text(pDX, IDC_EDIT1, m_id);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTmdDlg, CDialog)
	//{{AFX_MSG_MAP(CTmdDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_WM_RBUTTONUP()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmdDlg message handlers

BOOL CTmdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetWindowPos(NULL,0,0,1280,800,NULL);	
	long lIndex=-2030,l;
	char st[80];
	FILE *fp;
	hBrd= okOpenBoard(&lIndex);
	RECT rct;
	SetRect(&rct,0,0,0,0);
	okSetTargetRect(hBrd,BUFFER,&rct);
	SetRect(&rct,2,40,2+W2,40+HEIGHT);
//	SetRect(&rct,0,0,W2,HEIGHT);

	okSetTargetRect(hBrd,SCREEN,&rct);
//	l=okLoadImageFile(hBrd,"BMP\\a.bmp",0,BUFFER,0,1);
//	l=okLoadImageFile(hBrd,"BMP\\fr0.bmp",0,BUFFER,0,1);
	l=okLoadImageFile(hBrd,"JPG\\gt3_0.jpg",0,BUFFER,0,1);
	sprintf(st,"l=%d",l);m_lst.AddString(st);
	b0=(BYTE *)okGetBufferAddr(hBrd,0);//FillMemory(b0,HEIGHT*W2,0);
	b1=(BYTE *)okGetBufferAddr(hBrd,1);
	pnt=(PNT *)okGetBufferAddr(hBrd,0);
//	pnu=(PNT *)okGetBufferAddr(hBrd,1);
	CreateDirectory("DAT\\",NULL);
	CreateDirectory("JPG\\",NULL);
	m_cmb.SetCurSel(0);m_dot.SetCurSel(0);
	if(!(fp=fopen("DAT\\id.dat","rb"))){m_id=1427.5;UpdateData(false);}
	else{fread(&m_id,sizeof(float),1,fp);UpdateData(false);}

//	okConvertRect(hBrd,SCREEN,0,BUFFER,0,1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTmdDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTmdDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTmdDlg::OnButton1() 
{
	long isum,jsum;
	int i0,j0,k1,wk,kcnt;
	FXYZ fi,fj,*b1;
	FILE *fp;
	long k,l,n;
	int i,j,k0;
	char st[80];
	BLOCKINFO blk;
	float sum,bsum,gsum,rsum;

	sprintf(st,"BMP\\fr_%d.bmp",m_cmb.GetCurSel());m_lst.AddString (st);
	okLoadImageFile(hBrd,st,0,BUFFER,0,1);
	dsply();

PNT p;
//	sprintf(st,"BMP\\fr_%d.bmp",m_cmb.GetCurSel());m_lst.AddString (st);
	sprintf(st,"BMP\\fr_%d.bmp",0);m_lst.AddString (st);
	okLoadImageFile(hBrd,st,0,BUFFER,0,1);
	pnt=(PNT *)okGetBufferAddr(hBrd,0);
//	for(k=0;k<HEIGHT*W2;k++){if(pnt[k].g>0)pnt[k]=WHITE;}dsply();
	sum=bsum=gsum=rsum=0;
	for(k=0;k<HEIGHT*W2;k++){
		p=pnt[k];if(p.g==0)continue;
		sum++;bsum+=p.b;gsum+=p.g;rsum+=p.r;};
	fi.x=bsum/sum;fi.y=gsum/sum;fi.z=rsum/sum;
	sprintf(st,"bgr: %9.3f %9.3f %9.3f",fi.x,fi.y,fi.z);m_lst.AddString(st);

	sprintf(st,"BMP\\fr_%d.bmp",1);m_lst.AddString (st);
	okLoadImageFile(hBrd,st,0,BUFFER,0,1);
	pnt=(PNT *)okGetBufferAddr(hBrd,0);
//	for(k=0;k<HEIGHT*W2;k++){if(pnt[k].g>0)pnt[k]=WHITE;}dsply();
	sum=bsum=gsum=rsum=0;
	for(k=0;k<HEIGHT*W2;k++){
		p=pnt[k];if(p.g==0)continue;
		sum++;bsum+=p.b;gsum+=p.g;rsum+=p.r;};
	fj.x=bsum/sum;fj.y=gsum/sum;fj.z=rsum/sum;
	sprintf(st,"bgr: %9.3f %9.3f %9.3f",fj.x,fj.y,fj.z);m_lst.AddString(st);

	FXYZ rate;float b,g,r;

//	sprintf(st,"BMP\\fr_%d.bmp",m_cmb.GetCurSel());m_lst.AddString (st);
	sprintf(st,"BMP\\fr_%d.bmp",2);m_lst.AddString (st);
	okLoadImageFile(hBrd,st,0,BUFFER,0,1);
	pnt=(PNT *)okGetBufferAddr(hBrd,0);
//	for(k=0;k<HEIGHT*W2;k++){if(pnt[k].g>0)pnt[k]=WHITE;}dsply();
	sum=bsum=gsum=rsum=0;
	for(k=0;k<HEIGHT*W2;k++){
		p=pnt[k];if(p.g==0)continue;
		sum++;bsum+=p.b;gsum+=p.g;rsum+=p.r;};
	fi.x=bsum/sum;fi.y=gsum/sum;fi.z=rsum/sum;
	sprintf(st,"bgr: %9.3f %9.3f %9.3f",fi.x,fi.y,fi.z);m_lst.AddString(st);

	m_lst.AddString("here");
	m_lst.AddString("Finished");
return;
//===========================================================
	for(k=0;k<3;k++)
	{
		for(k0=0;k0<2;k0++){
			sprintf(st,"BMP\\F%d_%d.bmp",k,k0);//m_lst.AddString(st);
			l=okLoadImageFile(hBrd,st,0,BUFFER,0,1);
			sprintf(st,"l=%d",l);m_lst.AddString(st);
			for(l=0;l<HEIGHT*W2;l++){b0[l]=(b0[l*3+0]*114+b0[l*3+1]*587+b0[l*3+2]*299)/1000;}
			sum=0;
			for(l=0;l<HEIGHT*W2;l++)sum+=b0[l];
			sum/=(HEIGHT*W2);sum*=1.2;
			for(i=0;i<HEIGHT;i++)for(j=0;j<350;j++)b0[i*W2+j]=sum;
			for(i=0;i<HEIGHT;i++)for(j=W2-150;j<W2;j++)b0[i*W2+j]=sum;

			BYTE *bf=(BYTE *)new BYTE[H2*W4];
			BYTE *bw=(BYTE *)new BYTE[H2*W4];

			for(i=0;i<H2;i++)CopyMemory(&bf[i*W4],&b0[i*W4],W4);
			im2bw(bf,bw,H2,W4);
			for(i=0;i<H2;i++)CopyMemory(&b0[i*W4],&bw[i*W4],W4);

			for(i=0;i<H2;i++)CopyMemory(&bf[i*W4],&b0[(i+H2)*W4],W4);
			im2bw(bf,bw,H2,W4);
			for(i=0;i<H2;i++)CopyMemory(&b0[(i+H2)*W4],&bw[i*W4],W4);
			for(i=0;i<H2;i++)CopyMemory(&bf[i*W4],&b0[(i+H2*2)*W4],W4);
			im2bw(bf,bw,H2,W4);
			for(i=0;i<H2;i++)CopyMemory(&b0[(i+H2*2)*W4],&bw[i*W4],W4);
			for(i=0;i<H2;i++)CopyMemory(&bf[i*W4],&b0[(i+H2*3)*W4],W4);
			im2bw(bf,bw,H2,W4);
			for(i=0;i<H2;i++)CopyMemory(&b0[(i+H2*3)*W4],&bw[i*W4],W4);

			delete [] bf,bw;

//			okConvertRect(hBrd,SCREEN,0,TARGET(&blk),0,1);Sleep(1000);

//			sprintf(st,"JPG\\F%d_%d.jpg",k,k0);m_lst.AddString(st);
			sprintf(st,"JPG\\a%d.jpg",k*2+k0);m_lst.AddString(st);
			l=okSaveImageFile(hBrd,st,70,TARGET(&blk),0,1);
			sprintf(st,"l=%d",l);m_lst.AddString(st);

		}
	}

//=========================================


	if(!(fp=fopen("DAT\\dt0.dat","rb")))
	{
	fp=fopen("DAT\\dt0.dat","wb");
	for(k=0;k<6;k++)
	{
		
			m_lst.AddString("");m_lst.AddString("========================");
			sprintf(st,"JPG\\a%d.jpg",k);
			m_lst.AddString(st);
			l=okLoadImageFile(hBrd,st,0,BUFFER,0,1);
			for(n=0;n<HEIGHT*W2;n++)if(b0[n]>127)b0[n]=255;else b0[n]=0;

			int *bg5=(int *)new int[HEIGHT*W2];
			FillMemory(bg5,sizeof(int)*HEIGHT*W2,0);
			for(n=0;n<HEIGHT*W2;n++)if(b0[n]>0)bg5[n]=1;

			wk=BWLabel(bg5,HEIGHT,W2);
			sprintf(st,"wk=%d",wk);m_lst.AddString(st);
			kcnt=0;
			for(k1=1;k1<=wk;k1++){		
			sum=isum=jsum=0;
			for(i=0;i<HEIGHT;i++)for(j=0;j<W2;j++){
				if(bg5[i*W2+j]==k1)
				{
					sum++;
					isum+=i;jsum+=j;
				}
			}
			if(sum>5){
				fi.x=float(jsum)/float(sum);
				fi.y=float(isum)/float(sum);
				fi.z=sum;
				fwrite(&fi,sizeof(FXYZ),1,fp);
				sprintf(st,"%02d %3d | %9.3f %9.3f %9.3f | %03d",k1,sum,fi.x,fi.y,fi.z,kcnt++);m_lst.AddString(st);
				i0=fi.y;j0=fi.x;
				for(i=-2;i<=2;i++)for(j=-2;j<=2;j++)b0[(i+i0)*W2+j+j0]=128;
			}
		}
		delete [] bg5;		
		okConvertRect(hBrd,SCREEN,0,BUFFER,0,1);			
		
	
	}
	fclose(fp);
	return;
	}
//===========================================
	float af,x,y;
	FXYZ dt0[6*25];FXYZ dt1[25]; 
	fread(dt0,sizeof(FXYZ),6*25,fp);fclose(fp);
	for(k=0;k<6;k++){CopyMemory(dt1,&dt0[k*25],sizeof(FXYZ)*25);
	Sort7z(dt1,25,1);
	for(i=0;i<24;i++)
	{
		x=dt1[i].x-dt1[24].x;y=dt1[i].y-dt1[24].y;r=sqrt(x*x+y*y);
		af=asin(y/r);if(x<0)af=pi-af;dt1[i].z=af;

	}
	dt1[24].z=0;

	for(i=23;i>=0;i--)
	{
		dt1[i].z-=dt1[0].z;
		if(dt1[i].z<0)dt1[i].z+=2*pi;
	}
	Sort7z(dt1,24,0);
//	m_lst.AddString("");for(i=0;i<25;i++){sprintf(st,"%d %02d %9.3f %9.3f %9.4f",k,i,dt1[i].x,dt1[i].y,dt1[i].z*180/pi);m_lst.AddString(st);}

	sprintf(st,"DAT\\dt1_%d.dat",k);
	fp=fopen(st,"wb");fwrite(dt1,sizeof(FXYZ),25,fp);fclose(fp);

	}
	ELPS ep[6];
//===========================================
	for(k=0;k<6;k++){
		sprintf(st,"DAT\\dt1_%d.dat",k);
		if(!(fp=fopen(st,"rb")))return;
		fread(dt1,sizeof(FXYZ),25,fp);fclose(fp);
		m_lst.AddString("");for(i=0;i<25;i++){sprintf(st," %d %02d %9.3f %9.3f %9.4f",k,i,dt1[i].x,dt1[i].y,dt1[i].z*180/pi);m_lst.AddString(st);}
//		sprintf(st,"JPG\\a%d.jpg",k);okLoadImageFile(hBrd,st,0,BUFFER,0,1);for(wk=0;wk<25;wk++){i0=dt1[wk].y;j0=dt1[wk].x;for(i=-4;i<=4;i++)for(j=-4;j<=4;j++)b0[(i+i0)*W2+j+j0]=128;okConvertRect(hBrd,SCREEN,0,BUFFER,0,1);Sleep(20);}okConvertRect(hBrd,SCREEN,0,BUFFER,0,1);Sleep(1000);
		ep[k]=fitElp(dt1,24);sprintf(st,"%d %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f",k,ep[k].a,ep[k].b,ep[k].c.x,ep[k].c.y,ep[k].bt*180/pi,ep[k].ag*180/pi);m_lst.AddString(st);
	}
	fp=fopen("DAT\\ep.dat","wb");fwrite(ep,sizeof(ELPS),6,fp);fclose(fp);
//===========================================


//===========================================
	m_lst.AddString("here");
	m_lst.AddString("Finished");
	sprintf(st,"HEIGHT=%4d W2=%4d",HEIGHT,W2);m_lst.AddString(st);
}
FXYZ pdt2vct(FXYZ in,float &r)
{
	CTmdDlg *pDlg = (CTmdDlg *)(AfxGetApp()->GetMainWnd());
	FXYZ o;
	float x,y,z;
	x=in.x-W4;y=in.y-H2;z=pDlg->m_id;r=sqrt(x*x+y*y+z*z);
	o.x=x/r;o.y=y/r;o.z=z/r;
	return o;
}

//		LOGFONT lfLogFont={32, 24, 0, 0, 100, 0, 0, 0, 0, 1, 2, 1, 26, "Arial"};
//		LOGFONT lfLogFont={64, 48, 0, 0, 400, 0, 0, 0, 0, 1, 2, 1, 26, "Arial"};
//		SETTEXTMODE	textmode={0x000000,0xffffff,0,0,0};//°×é?
//		SETTEXTMODE	textmode={0x000000,0x0000ff,0,0,0};//oìé?
//		RECT rect0={30,20,0,0};
//		rect0.top=100;
//		sprintf(st,"|¤b=%7.2f mm",ep.b-RADIUS);
//		okSetTextTo(hBrd,TARGET(&blk),&rect0,&lfLogFont,&textmode,st,20);
void CTmdDlg::OnButton2(){
	char st[80];
	int k0,k1,i,j,wk,kcnt,i0,j0;
	PNT p;
	BYTE b;
	FILE *fp;
	float x,y,z,r;
	long k,n,sum;
	if(!(fp=fopen("JPG\\frbw1.bmp","rb"))){
	BLOCKINFO blk;
	BYTE *bf=(BYTE *)new BYTE [HEIGHT*W2];
	FillMemory(&blk,sizeof(blk),0);
	blk.iBitCount=8;
	blk.iFormType=FORM_GRAY8;
	blk.iHeight=HEIGHT;
	blk.iWidth=W2;
	blk.lpBits = bf;
	for(k0=0;k0<2;k0++){
		sprintf(st,"BMP\\fr_%d.bmp",k0);
		okLoadImageFile(hBrd,st,0,BUFFER,0,1);
		FillMemory(bf,HEIGHT*W2,255);
		for(k=0;k<HEIGHT*W2;k++)
		{
			p=pnt[k];
			if(p.b+p.g+p.r==0)bf[k]=0;else bf[k]=255;
		}
		sprintf(st,"JPG\\frbw%d.bmp",k0);m_lst.AddString(st);
		okConvertRect(hBrd,SCREEN,0,TARGET(&blk),0,1);Sleep(1000);
		okSaveImageFile(hBrd,st,100,TARGET(&blk),0,1);
	}


	delete [] bf;return;
	}

//=====================================
if(!(fp=fopen("JPG\\bw1.jpg","rb"))){

	int *bg5=(int *)new int[HEIGHT*W2];
	for(k0=0;k0<2;k0++){
		sprintf(st,"JPG\\frbw%d.bmp",k0);
		okLoadImageFile(hBrd,st,0,BUFFER,0,1);
			FillMemory(bg5,sizeof(int)*HEIGHT*W2,0);
			for(n=0;n<HEIGHT*W2;n++)if(b0[n]>0)bg5[n]=1;
			wk=BWLabel(bg5,HEIGHT,W2);
			sprintf(st,"wk=%d",wk);m_lst.AddString(st);
			kcnt=0;

			for(k1=1;k1<=wk;k1++){		
			sum=0;for(k=0;k<HEIGHT*W2;k++)if(bg5[k]==k1)sum++;
			if(sum<1000){for(k=0;k<HEIGHT*W2;k++)if(bg5[k]==k1)bg5[k]=0;}
			FillMemory(b0,HEIGHT*W2,0);
			for(k=0;k<HEIGHT*W2;k++)if(bg5[k]>0)b0[k]=255;
		}
		dsply();Sleep(1000);
		sprintf(st,"JPG\\bw%d.jpg",k0);
		okSaveImageFile(hBrd,st,70,BUFFER,0,1);
	}
	delete [] bg5;return;
}
//============================================
if(!(fp=fp=fopen("DAT\\sft.dat","rb"))){
POINT a[2];
long sumi,sumj;
	sum=sumi=sumj=0;
	for(k0=0;k0<2;k0++){
		sprintf(st,"JPG\\bw%d.jpg",k0);
		okLoadImageFile(hBrd,st,0,BUFFER,0,1);
		for(k=0;k<HEIGHT*W2;k++)if(b0[k]>127){sum++;sumi+=k/1280;sumj+=k%1280;}
		a[k0].x=sumj/sum;
		a[k0].y=sumi/sum;
		sprintf(st,"x=%4d y=%4d",a[k0].x,a[k0].y);m_lst.AddString(st);
//		dsply();Sleep(1000);
	}
	i0=a[1].y-a[0].y;j0=a[1].x-a[0].x;
	sprintf(st,"xi0=%4d yj0=%4d",i0,j0);m_lst.AddString(st);
	okLoadImageFile(hBrd,"JPG\\bw0.jpg",0,BUFFER,0,1);
	okLoadImageFile(hBrd,"JPG\\bw1.jpg",0,BUFFER,1,1);

	for(k=0;k<HEIGHT*W2;k++){
		if(b0[k]>127)b0[k]=85;else b0[k]=0;
		if(b1[k]>127)b1[k]=170;else b1[k]=0;
		i=k/1280;j=k%1280;
		i+=i0;j+=j0+20;

		b0[k]+=b1[i*W2+j];
	}
	dsply();
	okSaveImageFile(hBrd,"JPG\\sft.jpg",70,BUFFER,0,1);
	a[0].x=j0+20;a[0].y=i0;
	fp=fopen("DAT\\sft.dat","wb");fwrite(&a[0],sizeof(POINT),1,fp);fclose(fp);
	return;}
//====================================================
if(!(fp=fopen("DAT\\b31.dat","rb"))){
	for(k0=0;k0<2;k0++){
		sprintf(st,"BMP\\fr_%d.bmp",k0);
		okLoadImageFile(hBrd,st,0,BUFFER,0,1);pnt=(PNT *)okGetBufferAddr(hBrd,0);
		sprintf(st,"DAT\\b3%d.dat",k0);
		fp=fopen(st,"wb");fwrite(pnt,sizeof(PNT),HEIGHT*W2,fp);fclose(fp);
		dsply();Sleep(1000);
	}
}
if(!(fp=fopen("DAT\\gry1.dat","rb")))
{
	BYTE *bf=(BYTE *)new BYTE[HEIGHT*W2];
	for(k0=0;k0<2;k0++){
		sprintf(st,"DAT\\b3%d.dat",k0);
		fp=fopen(st,"rb");fread(pnt,sizeof(PNT),HEIGHT*W2,fp);fclose(fp);
		for(k=0;k<HEIGHT*W2;k++)bf[k]=(pnt[k].b*114+pnt[k].g*587+pnt[k].r*299)/1000;
		sprintf(st,"DAT\\gry%d.dat",k0);
		fp=fopen(st,"wb");fwrite(bf,1,HEIGHT*W2,fp);fclose(fp);
	}
	delete [] bf;
}
//	for(k0=0;k0<2;k0++){sprintf(st,"DAT\\gry%d.dat",k0);fp=fopen(st,"rb");fread(b0,1,HEIGHT*W2,fp);fclose(fp);dsply();Sleep(1000);}
//if(!(fp=fopen("DAT\\gv1.dat","rb")))
{
	float fwk,fsm;long l;
	BYTE *bf=(BYTE *)new BYTE[HEIGHT*W2];
	float *gv=(float *)new float[HEIGHT*W2];
	FillMemory(gv,HEIGHT*W2*4,0);
	int ix[8]={-W2-1,-W2,-W2+1,-1,1,W2-1,W2,W2+1};
	for(k0=0;k0<2;k0++){
		sprintf(st,"DAT\\b3%d.dat",k0);
		if(!(fp=fopen(st,"rb")))return;fread(pnt,sizeof(PNT),HEIGHT*W2,fp);fclose(fp);
		sprintf(st,"DAT\\gry%d.dat",k0);
		if(!(fp=fopen(st,"rb")))return;fread(bf,1,HEIGHT*W2,fp);fclose(fp);
		FillMemory(gv,4,HEIGHT*W2,0);
		for(k=0;k<HEIGHT*W2;k++){
			if(bf[k]<=0){continue;}
			fsm=0;
			for(k1=0;k1<8;k1++){
				l=k+ix[k1];
				if(l>=0&&l<HEIGHT*W2){
					fwk=float(bf[l]-bf[k]);
					fsm+=fabs(fwk/bf[k]);
				}
				gv[k]=fsm;
			}
		}
		sprintf(st,"DAT\\gv%d.dat",k0);
		fp=fopen(st,"wb");fwrite(gv,4,HEIGHT*W2,fp);fclose(fp);
		m_lst.AddString("here")	
			;
long cnt;
cnt=0;for(k=0;k<HEIGHT*W2;k++)if(gv[k]==0)cnt++;
sprintf(st,"cnt=%d",cnt);m_lst.AddString(st);

	}


	delete [] bf,gv;
	return;
}
//====================================================


	m_lst.AddString("here");

/*if(!(fp=fp=fopen("DAT\\gw1.dat","rb"))){
	int ix[8];ix[0]=-W2-1;ix[1]=-W2;ix[2]=-W2+1;ix[3]=-1,ix[4]=1,ix[5]=W2-1;ix[6]=W2;ix[7]=W2+1;
	float *gv=(float *)new	float[HEIGHT*W2],f;
	float *gw=(float *)new	float[HEIGHT*W2];
	BYTE *bf=(BYTE *)new	BYTE[HEIGHT*W2];
	for(k0=0;k0<2;k0++){

		FillMemory(gw,sizeof(float)*HEIGHT*W2,0);
		FillMemory(gv,sizeof(float)*HEIGHT*W2,0);
		sprintf(st,"DAT\\bf%d.dat",k0);
		fp=fopen(st,"rb");fread(bf,1,HEIGHT*W2,fp);fclose(fp);
		sprintf(st,"BMP\\fr_%d.bmp",k0);
		okLoadImageFile(hBrd,st,0,BUFFER,0,1);

		for(k=0;k<HEIGHT*W2;k++){
		p=pnt[k];
		if(p.b+p.g+p.r>0)gv[k]=(114*p.b+587*p.g+299*p.r)/10;}

		for(k=0;k<HEIGHT*W2;k++){
			for(k1=0;k1<8;k1++){
				if(k+ix[k1]>=0&&k+ix[k1]<HEIGHT*W2){
				f=fabs((gv[k+ix[k1]]-gv[k])/gv[k]);
				gw[k]+=f;}
			}
		}

		sprintf(st,"DAT\\gw%d.dat",k0);
		fp=fopen(st,"wb");fwrite(gw,4,HEIGHT*W2,fp);fclose(fp);

		sprintf(st,"DAT\\gv%d.dat",k0);
		fp=fopen(st,"wb");fwrite(gv,4,HEIGHT*W2,fp);fclose(fp);
	}
	delete [] gv,gw,bf;return;
	}*/
//===================================
//	for(k0=0;k0<2;k0++){sprintf(st,"BMP\\fr_%d.bmp",k0);okLoadImageFile(hBrd,st,0,BUFFER,0,1);pnt=(PNT *)okGetBufferAddr(hBrd,0);for(k=0;k<HEIGHT*W2;k++){b=pnt[k].g;pnt[k].b=pnt[k].r=pnt[k].g=b;;}sprintf(st,"BMP\\bw%d.bmp",k0);okSaveImageFile(hBrd,st,100,BUFFER,0,1);dsply();Sleep(1000);}return;
/*
	float *gw=(float *)new	float[HEIGHT*W2];
	for(k0=0;k0<2;k0++){
		sprintf(st,"DAT\\gw%d.dat",k0);
		if(!(fp=fopen(st,"rb")))return;fread(gw,sizeof(float),HEIGHT*W2,fp);fclose(fp);
//		sprintf(st,"BMP\\bw%d.bmp",k0);
//		okLoadImageFile(hBrd,st,0,BUFFER,0,1);pnt=(PNT *)okGetBufferAddr(hBrd,0);
//		for(k=0;k<HEIGHT*W2;k++){if(gw[k]>0.01)pnt[k]=RED;}

		sprintf(st,"JPG\\bw%d.jpg",k0);
		okLoadImageFile(hBrd,st,0,BUFFER,0,1);b0=(BYTE *)okGetBufferAddr(hBrd,0);

		for(k=0;k<HEIGHT*W2;k++){if(gw[k]>1.01)b0[k]=127;}
//		for(i=0;i<HEIGHT;i++)for(j=0;j<W2;j++){if(gw[i*W2+j]>1.01)b0[i*W2+j]=127;}

		dsply();Sleep(1000);
	}
	delete [] gw;*/


//		sprintf(st,"BMP\\bw%d.bmp",k0);
//		okLoadImageFile(hBrd,st,0,BUFFER,0,1);pnt=(PNT *)okGetBufferAddr(hBrd,0);
//		for(k=0;k<HEIGHT*W2;k++){if(gw[k]>0.01)pnt[k]=RED;}

//		sprintf(st,"JPG\\bw%d.jpg",k0);
//		okLoadImageFile(hBrd,st,0,BUFFER,0,1);b0=(BYTE *)okGetBufferAddr(hBrd,0);
//		for(k=0;k<HEIGHT*W2;k++)if(b0[k]>127)b0[k]=255;else b0[k]=0;
//		for(k=0;k<HEIGHT*W2;k++)bf[k]=b0[k];
//		sprintf(st,"DAT\\bf%d.dat",k0);
//		fp=fopen(st,"wb");fwrite(bf,4,HEIGHT*W2,fp);fclose(fp);
//		CopyMemory(bf,b0,HEIGHT*W2);
//		FillMemory(b0,HEIGHT*W2,0);
//		sprintf(st,"DAT\\bf%d.dat",k0);
//		fp=fopen(st,"rb");fread(bf,4,HEIGHT*W2,fp);fclose(fp);
//		for(k=0;k<HEIGHT*W2;k++)b0[k]=bf[k];

/*		sprintf(st,"JPG\\bw%d.jpg",k0);
		okLoadImageFile(hBrd,st,0,BUFFER,0,1);b0=(BYTE *)okGetBufferAddr(hBrd,0);
			for(k=0;k<HEIGHT*W2;k++)if(b0[k]>127)b0[k]=255;else b0[k]=0;
			FillMemory(bg5,sizeof(int)*HEIGHT*W2,0);
			for(n=0;n<HEIGHT*W2;n++)if(b0[n]>0)bg5[n]=1;
			wk=BWLabel(bg5,HEIGHT,W2);
			sprintf(st,"wk=%d",wk);m_lst.AddString(st);
			kcnt=0;
			for(k1=1;k1<=wk;k1++){		
			sum=0;for(k=0;k<HEIGHT*W2;k++)if(bg5[k]==k1)sum++;
			if(sum<100){for(k=0;k<HEIGHT*W2;k++)if(bg5[k]==k1)bg5[k]=0;}
			FillMemory(b0,HEIGHT*W2,0);
			for(k=0;k<HEIGHT*W2;k++)if(bg5[k]>0)b0[k]=255;
		}
		dsply();Sleep(1000);
		sprintf(st,"JPG\\bw%d.jpg",k0);
		okSaveImageFile(hBrd,st,70,BUFFER,0,1);*/




//		for(k=0;k<HEIGHT*W2;k++){if(gw[k]>1.01)b0[k]=127;}
//		for(i=0;i<HEIGHT;i++)for(j=0;j<W2;j++){if(gw[i*W2+j]>1.01)b0[i*W2+j]=127;}

}



;


/*void CTmdDlg::OnButton2() 
{
	char st[80];
	int i,j,k0,k,i0,j0;
	FILE *fp;
	FXYZ fi;
	ELPS ep[6];
	float af,x,y,xp,yp;
		LOGFONT lfLogFont={32, 24, 0, 0, 100, 0, 0, 0, 0, 1, 2, 1, 26, "Arial"};
//		LOGFONT lfLogFont={64, 48, 0, 0, 400, 0, 0, 0, 0, 1, 2, 1, 26, "Arial"};
		SETTEXTMODE	textmode={0x000000,0xffffff,0,0,0};//°×é?
//		SETTEXTMODE	textmode={0x000000,0x0000ff,0,0,0};//oìé?
	if(!(fp=fopen("DAT\\dt.dat","rb")))return;
	fread(dt,12,150,fp);fclose(fp);
//	for(k=0;k<6;k++){m_lst.AddString("");for(k0=0;k0<25;k0++){fi=dt[k][k0];sprintf(st,"%02d %9.3f %9.3f %9.3f",k0,fi.x,fi.y,fi.z);m_lst.AddString(st);}}
	RECT rect0={30,20,0,0};
	for(k=0;k<6;k++){
		sprintf(st,"JPG\\sl%d.jpg",k);
		okLoadImageFile(hBrd,st,0,BUFFER,0,1);
//		for(k0=0;k0<25;k0++){i0=dt[k][k0].y;j0=dt[k][k0].x;for(i=-2;i<=2;i++)for(j=-2;j<=2;j++)b0[(i0+i)*W2+j0+j]=0;dsply();Sleep(50);}
		dsply();Sleep(1000);
		ep[k]=fitElp(dt[k],24);
		sprintf(st,"%d %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f",k,ep[k].a,ep[k].b,ep[k].c.x,ep[k].c.y,ep[k].bt*180/pi,ep[k].ag*180/pi);m_lst.AddString(st);
//		m_lst.AddString("");
		i0=ep[k].c.y;j0=ep[k].c.x;
		for(i=-2;i<=2;i++)for(j=-2;j<=2;j++)b0[(i0+i)*W2+j0+j]=0;
		dsply();Sleep(1000);
//x=a*cos(af)*cos(bt)-b*sin(af)*sin(bt)+xc
//y=a*cos(af)*sin(bt)+b*sin(af)*cos(bt)+yc

//fi.y=a*cos(af)*sin(bt)+b*sin(af)*cos(bt)+yc;	
//fi.x=a*cos(af)*cos(bt)-b*sin(af)*sin(bt)+xc;
		
//			af=(float)i*pi/360.;
//			y=ep.b*sin(af)/5.;
//			x=ep.a*cos(af)/5.;
//			xp=x*cos(ep.bt)+y*sin(ep.bt);
//			yp=-x*sin(ep.bt)+y*cos(ep.bt);
//			i1=int(yp+h2);
//			j1=int(xp+w2);


		for(k0=0;k0<360;k0++){
			af=k0*180/pi;
			y=ep[k].b*sin(af);
			x=ep[k].a*cos(af);
			xp=x*cos(ep[k].bt)+y*sin(ep[k].bt);
			yp=-x*sin(ep[k].bt)+y*cos(ep[k].bt);
			fi.x=xp+ep[k].c.x;
			fi.y=yp+ep[k].c.y;
		
			sprintf(st,"%03d %9.3f %9.3f",k0,fi.x,fi.y);m_lst.AddString(st);
			i=fi.y;j=fi.x;
			if(j>0&&i>0)b0[i*W2+j]=255;
		}

		rect0.top=20;
		sprintf(st,"bt=%9.4f",ep[k].bt*180/pi);
		okSetTextTo(hBrd,BUFFER,&rect0,&lfLogFont,&textmode,st,20);
		for(i=0;i<HEIGHT;i+=2)b0[i*W2+W4]=255;
		for(j=0;j<W2;j+=2)b0[H2*W2+j]=255;
		for(k0=-300;k0<300;k0++){

			fi.x=ep[k].c.x+k0*cos(ep[k].bt);
			fi.y=ep[k].c.y+k0*sin(ep[k].bt);
			i=fi.y;j=fi.x;
			b0[i*W2+j]=255;
		}
		for(k0=-300;k0<300;k0++){
			fi.x=ep[k].c.x+k0*cos(ep[k].bt+pi/2);
			fi.y=ep[k].c.y+k0*sin(ep[k].bt+pi/2);
			i=fi.y;j=fi.x;
			b0[i*W2+j]=0;
		}

		dsply();Sleep(1000);
		sprintf(st,"JPG\\se%d.jpg",k);
		okSaveImageFile(hBrd,st,70,BUFFER,0,1);
	}

	k0=m_cmb.GetCurSel();
	sprintf(st,"JPG\\sl%d.jpg",k0);
	okLoadImageFile(hBrd,st,0,BUFFER,0,1);
	for(i=0;i<rct0.top ;i++)for(j=0;j<W2;j++)b0[i*W2+j]=gray;
	for(i=rct0.bottom;i<HEIGHT ;i++)for(j=0;j<W2;j++)b0[i*W2+j]=gray;
	for(j=0;j<rct0.left ;j++)for(i=0;i<HEIGHT;i++)b0[i*W2+j]=gray;	
	for(j=rct0.right;j<W2 ;j++)for(i=0;i<HEIGHT;i++)b0[i*W2+j]=gray;	
	okConvertRect(hBrd,SCREEN,0,BUFFER,0,1);
	sprintf(st,"JPG\\s%d.jpg",k0);
	okSaveImageFile(hBrd,st,70,BUFFER,0,1);
	k0++;k0%=6;
	m_cmb.SetCurSel(k0);

}*/
/*
void CTmdDlg::OnButton2() 
{
	int i,i0,j,j0,k0,k1;
	long l,n,k;
	char st[80];
	FILE *fp;
	ELPS ep[6];
	float a,r,af,bt,gm,t0[3],la,lb,Q[16],A[9],Qinv[16],U[16];
	FXYZ du[6][25],fi,t[6],tt[3],fj,fk;
//===========================================
	m_lst.ResetContent();
	if(!(fp=fopen("DAT\\ep.dat","rb")))return;
	fread(ep,sizeof(ELPS),6,fp);fclose(fp);
//	for(k=0;k<6;k++){sprintf(st,"%d %9.3f %9.3f %9.3f %9.3f %9.3f %9.3f",k,ep[k].a,ep[k].b,ep[k].c.x,ep[k].c.y,ep[k].bt*180/pi,ep[k].ag*180/pi);m_lst.AddString(st);}
//	a=(RADIUS/ep[2].a-RADIUS/ep[0].a)*m_id;sprintf(st,"a=%9.3f",a);m_lst.AddString(st);a=(RADIUS/ep[4].a-RADIUS/ep[2].a)*m_id;sprintf(st,"a=%9.3f",a);m_lst.AddString(st);a=(RADIUS/ep[3].a-RADIUS/ep[1].a)*m_id;sprintf(st,"a=%9.3f",a);m_lst.AddString(st);a=(RADIUS/ep[5].a-RADIUS/ep[3].a)*m_id;sprintf(st,"a=%9.3f",a);m_lst.AddString(st);

	for(k=0;k<6;k++){
		fi.x=ep[k].c.x;fi.y=ep[k].c.y;
		fi=pdt2vct(fi,r);
		a=RADIUS*r/ep[k].a;
//		sprintf(st,"%9.6f %9.6f %9.6f %9.3f %9.3f",fi.x,fi.y,fi.z,r,a);m_lst.AddString(st);
		t[k].x=a*fi.x;t[k].y=a*fi.y;t[k].z=a*fi.z;
//		sprintf(st," %9.3f %9.3f %9.3f",t[k].x,t[k].y,t[k].z);m_lst.AddString(st);
	}
//	fi.x=fi.y=fi.z=3;fj.x=fj.y=fj.z=4;fk=sbstrct(fi,fj);sprintf(st,"%9.3f %9.3f %9.3f ",fk.x,fk.y,fk.z);m_lst.AddString(st);

//	fi=sbstrct(t[1],t[0]);sprintf(st,"%9.3f %9.3f %9.3f",fi.x,fi.y,fi.z);m_lst.AddString(st);
//	fi=sbstrct(t[3],t[2]);sprintf(st,"%9.3f %9.3f %9.3f",fi.x,fi.y,fi.z);m_lst.AddString(st);
//	fi=sbstrct(t[5],t[4]);sprintf(st,"%9.3f %9.3f %9.3f",fi.x,fi.y,fi.z);m_lst.AddString(st);
	tt[0]=sbstrct(t[1],t[0]);
	tt[1]=sbstrct(t[3],t[2]);
	tt[2]=sbstrct(t[5],t[4]);
	t0[0]=a=mods(tt[0]);//sprintf(st,"%9.3f",a);m_lst.AddString(st);
	t0[1]=a=mods(tt[1]);//sprintf(st,"%9.3f",a);m_lst.AddString(st);
	t0[2]=a=mods(tt[2]);//sprintf(st,"%9.3f",a);m_lst.AddString(st);

	fp=fopen("DAT\\t.dat","wb");fwrite(t,sizeof(FXYZ),6,fp);fclose(fp);
	fp=fopen("DAT\\tt.dat","wb");fwrite(tt,sizeof(FXYZ),3,fp);fclose(fp);

	for(k=0;k<6;k++){sprintf(st,"DAT\\dt1_%d.dat",k);if(!(fp=fopen(st,"rb")))return;fread(du[k],12,25,fp);fclose(fp);}
//	for(k=0;k<6;k++){sprintf(st,"JPG\\a%d.jpg",k);m_lst.AddString("");l=okLoadImageFile(hBrd,st,0,BUFFER,0,1);for(n=0;n<HEIGHT*W2;n++)if(b0[n]>127)b0[n]=255;else b0[n]=0;sprintf(st,"%d %9.3f %9.3f",k,ep[k].c.x,ep[k].c.y);m_lst.AddString(st);i0=ep[k].c.y;j0=ep[k].c.x;for(i=-3;i<=3;i++)for(j=-3;j<=3;j++)b0[(i0+i)*W2+j0+j]=127;okConvertRect(hBrd,SCREEN,0,BUFFER,0,1);Sleep(1000);for(k0=0;k0<25;k0++){fi=du[k][k0];sprintf(st,"%02d %9.3f %9.3f %9.3f",k0,fi.x,fi.y,fi.z);m_lst.AddString(st);}}
	for(k=0;k<6;k+=2)
	{
//		sprintf(st," %9.3f %9.3f %9.3f|%9.3f %9.3f %9.3f",t[k].x,t[k].y,t[k].z,t[k+1].x,t[k+1].y,t[k+1].z);m_lst.AddString(st);
//		sprintf(st," %9.3f %9.3f %9.3f",tt[k/2].x,tt[k/2].y,tt[k/2].z);m_lst.AddString(st);
		fk=tt[k/2];fk=guiy(fk);
//		sprintf(st," %9.6f %9.6f %9.6f t0=%9.3f",fk.x,fk.y,fk.z,t0[k/2]);m_lst.AddString(st);
//		fk=pdt2vct(tt[k/2],r);
//		sprintf(st," %9.6f %9.6f %9.6f %9.3f",fk.x,fk.y,fk.z,r);m_lst.AddString(st);
//		m_lst.AddString("");
		for(k0=0;k0<25;k0++)
		{
			fi=du[k][k0];fj=du[k+1][k0];
			fi=pdt2vct(fi,r);fj=pdt2vct(fj,r);
//			sprintf(st,"%9.3f %9.3f %9.3f|%9.3f %9.3f %9.3f",fi.x,fi.y,fi.z,fj.x,fj.y,fj.z);m_lst.AddString(st);
//			sprintf(st,"%9.6f %9.6f %9.6f|%9.6f %9.6f %9.6f",fi.x,fi.y,fi.z,fj.x,fj.y,fj.z);m_lst.AddString(st);
			gm=acos(fi.x*fj.x+fi.y*fj.y+fi.z*fj.z);
			fi=du[k][k0];
			fi=pdt2vct(fi,r);
			af=acos(fi.x*fk.x+fi.y*fk.y+fi.z*fk.z);
			bt=pi-af-gm;
//			sprintf(st,"%02d af=%9.4f bt=%9.4f %9.4f",k0,af*180/pi,bt*180/pi,gm*180/pi);m_lst.AddString(st);
			la=t0[k/2]/sin(gm)*sin(bt);
			lb=t0[k/2]/sin(gm)*sin(af);
//			sprintf(st,"%02d %9.4f %9.4f",k0,la,lb);m_lst.AddString(st);
			fi.x*=la;fi.y*=la;fi.z*=la;
			fj.x*=lb;fj.y*=lb;fj.z*=lb;
//			sprintf(st,"%9.3f %9.3f %9.3f|%9.3f %9.3f %9.3f",fi.x,fi.y,fi.z,fj.x,fj.y,fj.z);m_lst.AddString(st);
			du[k][k0]=fi;du[k+1][k0]=fj;
		}
	}

//==================================================
	for(k=0;k<6;k++){
//		m_lst.AddString("");for(k0=0;k0<25;k0++){fi=du[k][k0];sprintf(st,"%02d %9.3f %9.3f %9.3f",k0,fi.x,fi.y,fi.z);m_lst.AddString(st);}
		cld2eig(du[k],25,Q,A);
//		Q[3]=t[k].x;Q[7]=t[k].y;Q[11]=t[k].z;
		sprintf(st,"t %9.3f %9.3f %9.3f",t[k].x,t[k].y,t[k].z);m_lst.AddString(st);
		m_lst.AddString("Q:");for(i=0;i<4;i++){sprintf(st,"%9.6f %9.6f %9.6f %9.3f",Q[i*4+0],Q[i*4+1],Q[i*4+2],Q[i*4+3]);m_lst.AddString(st);}
		m_lst.AddString("A:");sprintf(st,"%9.3f %9.3f %9.3f",A[0],A[4],A[8]);m_lst.AddString(st);
		sprintf(st,"DAT\\Q%d.dat",k);
		fopen(st,"wb");fwrite(Q,64,1,fp);fclose(fp);
	}

	for(k=0;k<6;k++){
//		m_lst.AddString("");for(k0=0;k0<25;k0++){fi=du[k][k0];sprintf(st,"%02d %9.3f %9.3f %9.3f",k0,fi.x,fi.y,fi.z);m_lst.AddString(st);}
		m_lst.AddString("");
		for(k0=0;k0<24;k0++){
		fi=sbstrct(du[k][24],du[k][k0]);fk=fj=sbstrct(du[k][24],du[k][(k0+6)%24]);fi=guiy(fi);fj=guiy(fj);
		af=acos(fi.x*fj.x+fi.y*fj.y+fi.z*fj.z);a=mods(fk);
		sprintf(st,"af %9.4f r=%9.3f",af*180/pi,a);m_lst.AddString(st);
		}
	}


		if(!(fp=fopen("DAT\\Q0.dat","rb")))return;fread(Q,64,1,fp);fclose(fp);
		if(!(fp=fopen("DAT\\Q1.dat","rb")))return;fread(Qinv,64,1,fp);fclose(fp);inv4(Qinv);
		mmtrx4(Q,Qinv,U);
		fp=fopen("DAT\\Q01.dat","wb");fwrite(U,64,1,fp);fclose(fp);

		if(!(fp=fopen("DAT\\Q2.dat","rb")))return;fread(Q,64,1,fp);fclose(fp);
		if(!(fp=fopen("DAT\\Q3.dat","rb")))return;fread(Qinv,64,1,fp);fclose(fp);inv4(Qinv);
		mmtrx4(Q,Qinv,U);
		fp=fopen("DAT\\Q23.dat","wb");fwrite(U,64,1,fp);fclose(fp);

		if(!(fp=fopen("DAT\\Q4.dat","rb")))return;fread(Q,64,1,fp);fclose(fp);
		if(!(fp=fopen("DAT\\Q5.dat","rb")))return;fread(Qinv,64,1,fp);fclose(fp);inv4(Qinv);
		mmtrx4(Q,Qinv,U);
		fp=fopen("DAT\\Q45.dat","wb");fwrite(U,64,1,fp);fclose(fp);

//==================================================
	m_lst.AddString("");
	m_lst.AddString("Finished");
}
*/

void CTmdDlg::OnButton3() 
{
	char st[80];
	long l,k,cnt;;
	int i,j,k0,k1,i0,j0,wk;
	FILE *fp;
if(!(fp=fopen("DAT\\gvsrc1.dat","rb")))
{
	for(k0=0;k0<2;k0++){
	float *gv=(float *)new float[HEIGHT*W2];
		FillMemory(gv,4*HEIGHT*W2,0);
//		sprintf(st,"JPG\\bw%d.jpg",k0);okLoadImageFile(hBrd,st,0,BUFFER,0,1);b0=(BYTE *)okGetBufferAddr(hBrd,0);
		FillMemory(b0,HEIGHT*W2,0);
		sprintf(st,"DAT\\gv%d.dat",k0);
		if(!(fp=fopen(st,"rb")))return;;
		fread(gv,4,HEIGHT*W2,fp);fclose(fp);
		cnt=0;
		for(k=0;k<HEIGHT*W2;k++){
			if(gv[k]<1)gv[k]=0;else cnt++;
		}
//		sprintf(st,"JPG\\gt3_%d.jpg",k0);okSaveImageFile(hBrd,st,70,BUFFER,0,1);
		sprintf(st,"%d  %5d",k0,cnt);m_lst.AddString(st);

		sprintf(st,"DAT\\gvsrc%d.dat",k0);
		fp=fopen(st,"wb");
		fwrite(gv,4,HEIGHT*W2,fp);
		fclose(fp);
//		dsply();Sleep(1000);
	delete [] gv;
	}
}
//===================================
POINT s;
float min,f,la,lb,La,Lb,lc,af,bt,gm;
long l0;
FXYZ fi,fj,fk,tt;
	if(!(fp=fopen("DAT\\tt.dat","rb")))return;
	fread(&tt,12,1,fp);fclose(fp);
	tt=guiy1(tt,lc);

	if(!(fp=fopen("DAT\\sft.dat","rb")))return;
	fread(&s,sizeof(POINT),1,fp);fclose(fp);
	sprintf(st,"x=%d y=%d",s.x,s.y);m_lst.AddString(st);

int ix[9]={-W2-1,-W2,-W2+1,-1,0,1,W2-1,W2,W2+1};
	if(!(fp=fopen("DAT\\gvsrc0.dat","rb")))return;;
	float *gv=(float *)new float[HEIGHT*W2];
	fread(gv,4,HEIGHT*W2,fp);fclose(fp);
	if(!(fp=fopen("DAT\\gvsrc1.dat","rb")))return;;
	float *gu=(float *)new float[HEIGHT*W2];
	fread(gu,4,HEIGHT*W2,fp);fclose(fp);
	cnt=0;F1.clear();
	for(k=0;k<HEIGHT*W2;k++){
		if(gv[k]==0)continue;
		i=k/1280+s.y;j=k%1280+s.x;
		l=i*W2+j;
		min=99999;
		for(k0=0;k0<9;k0++){
			f=fabs(gv[k]-gu[l]);
			if(f<min){
				min=f;l0=l;
			}
		}
		if(min>0.02)continue;
//		sprintf(st,"cnt=%04d k=%7d l0=%7d min=%9.6f",cnt++,k,l0,min);m_lst.AddString(st);
		fi.x=k;fi.y=l0;fi.z=min;
		F1.push_back(fi);
	}
//	fp=fopen("DAT\\cld0.dat","wb");
	for(k=0;k<F1.size();k++){
		fk=F1[k];
		fi.x=int(fk.x)%1280;fi.y=int(fk.x)/1280;
		fi=pdt2vct(fi,la);
		fk=F1[k];
		fj.x=int(fk.y)%1280;fj.y=int(fk.y)/1280;;
		fj=pdt2vct(fj,lb);
//		sprintf(st,"%05d %9.6f %9.6f %9.6f |%9.3f",k,fi.x,fi.y,fi.z,la);m_lst.AddString(st);
//		sprintf(st,"%05d %9.6f %9.6f %9.6f |%9.3f",k,fj.x,fj.y,fj.z,lb);m_lst.AddString(st);

		gm=acos(fi.x*fj.x+fi.y*fj.y+fi.z*fj.z);
		af=acos(fi.x*tt.x+fi.y*tt.y+fi.z*tt.z);
		bt=pi-af-gm;
//		sprintf(st,"%05d %9.4f %9.4f %9.4f",k,af*180/pi,bt*180/pi,gm*180/pi);m_lst.AddString(st);
		La=sin(bt)*lc/sin(gm);
		Lb=sin(af)*lc/sin(gm);
		fi.x*=La;fi.y*=La;fi.z*=La;F1[k]=fi;
		fj.x*=Lb;fj.y*=Lb;fj.z*=Lb;
		sprintf(st,"%05d %9.3f %9.3f %9.3f |%9.3f",k,fi.x,fi.y,fi.z,La);m_lst.AddString(st);
//		sprintf(st,"%05d %9.3f %9.3f %9.3f |%9.3f",k,fj.x,fj.y,fj.z,Lb);m_lst.AddString(st);			
//		fwrite(&fi,12,1,fp);
//		fwrite(&fj,12,1,fq);
	}
//	fclose(fp);
	fi.x=fi.y=fi.z=0;
	for(k=0;k<F1.size();k++){
		fi.x+=F1[k].x;
		fi.y+=F1[k].y;
		fi.z+=F1[k].z;		
	}
	fi.x/=F1.size();
	fi.y/=F1.size();
	fi.z/=F1.size();	
	for(k=0;k<F1.size();k++){
		F1[k].x-=fi.x;
		F1[k].y-=fi.y;
		F1[k].z-=fi.z;		
	}
	fp=fopen("DAT\\cld0.dat","wb");	
	for(k=0;k<F1.size();k++)fwrite(&F1[k],12,1,fp);fclose(fp);

	delete [] gv,gu;
	m_lst.AddString("");m_lst.AddString("finished");
}

bool maxLcl(float *gv,long k,float &max){
	int ix[8]={-W2-1,-W2,-W2+1,-1,1,W2-1,W2,W2+1},k0;
	long l;
	max=gv[k];
	if(max<=0)return false;
	for(k0=0;k0<8;k0++){
		l=k+ix[k0];
		if(gv[l]<=0)return false;
		if(max<=gv[l])return false;
	}
	return true;
}

FXYZ maxNine(float *gv,long k,float *co){
	CTmdDlg *pDlg = (CTmdDlg *)(AfxGetApp()->GetMainWnd());char st[80];
	FXYZ fi={0};
	int ix[9]={-W2-1,-W2,-W2+1,-1,0,1,W2-1,W2,W2+1},i,j,k0,ret;
	float Y[9];
	FXYZ f9[9];
	long l;
	FILE *fp;
	float XX[42]={0},x,y,z;
	float XT[6][9]={0};


	if(!(fp=fopen("DAT\\XX.dat","rb")))return fi;;fread(XX,4,42,fp);fclose(fp);
	if(!(fp=fopen("DAT\\XT.dat","rb")))return fi;fread(XT,4,56,fp);fclose(fp);

	for(i=0;i<9;i++){l=k+ix[i];Y[i]=gv[l];
//	sprintf(st,"Y %9.5f",Y[i]);pDlg->m_lst.AddString(st);
	}


	for(k=0;k<6;k++){for(i=0;i<9;i++){XX[36+k]+=XT[k][i]*Y[i];}}
	for(i=0;i<7;i++){
		sprintf(st,"XX %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f ",
			XX[i*6],XX[i*6+1],XX[i*6+2],XX[i*6+3],XX[i*6+4],XX[i*6+5]);
		pDlg->m_lst.AddString(st);
	}
	
	pDlg->m_lst.AddString("");
	ret=all_gauss(6,XX,&XX[36]);

	for(i=0;i<7;i++){
		sprintf(st,"XX %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f ",
			XX[i*6],XX[i*6+1],XX[i*6+2],XX[i*6+3],XX[i*6+4],XX[i*6+5]);
		pDlg->m_lst.AddString(st);
	}

//	sprintf(st,"ret=%d",ret);pDlg->m_lst.AddString(st);
	CopyMemory(co,&XX[36],4*6);
	for(i=0;i<6;i++){sprintf(st,"co %9.5f",XX[36+i]);pDlg->m_lst.AddString(st);}
	for(k0=0;k0<9;k0++)
	{
		f9[k0].y=(k0/3)-1;
		f9[k0].x=(k0%3)-1;
		f9[k0].z=Y[k0];
		x=f9[k0].x;y=f9[k0].y;
		z=co[0]*x*x+co[1]*x*y+co[2]*y*y+co[3]*x+co[4]*y+co[5];
//		sprintf(st,"%5.0f %5.0f %9.5f",f9[k0].x,f9[k0].y,f9[k0].z);pDlg->m_lst.AddString(st);
		sprintf(st,"z=%9.5f Y=%9.5f",z,f9[k0].z);pDlg->m_lst.AddString(st);
	}


//a*x+b*y+c=0;
//d*x+e*y+f=0;
//              x=-(c+b*y)/a;
//              y=[c*(d/a)-f]/[e-b*(d/a)];
//=============================
//a*x+b*y+d=0;
//b*x+c*y+e=0;
//              x=-(c+b*y)/a;
//              y=[c*(d/a)-f]/[e-b*(d/a)];

//              x=-(d+b*y)/a;
//              y=(d*(b/a)-e)/(c-b*(b/a));

//              fi.y=(co[3]*(co[1]/co[0])-co[4])/(co[2]-co[1]*(co[1]/co[0]));
//              fi.x=-(co[3]+co[1]*fi.y)/co[0];	
	


//     fi.y=(co[3]*(co[1]/co[0])-co[4])/(co[2]-co[1]*(co[1]/co[0])); 
//     fi.x=-(co[3]+co[1]*fi.y)/co[0];
//	 fi.z=co[0]*fi.x*fi.x+co[1]*fi.x*fi.y+co[2]*fi.y*fi.y+co[3]*fi.x+co[4]*fi.y+co[5];
//	 sprintf(st,"xyz  %9.3f %9.3f %9.5f",fi.x,fi.y,fi.z);pDlg->m_lst.AddString(st);

     fi.y=(co[3]*(co[1]/co[0])-co[4])/(co[2]-co[1]*(co[1]/co[0]));
     fi.x=-(co[3]+co[1]*fi.y)/co[0];
	 x=fi.x;y=fi.y;
	 fi.z=co[0]*x*x+
			co[1]*x*y+
			co[2]*y*y+
			co[3]*x+
			co[4]*y+	
			co[5];
sprintf(st,"fi %9.3f %9.3f %9.3f",fi.x,fi.y,fi.z);pDlg->m_lst.AddString(st);

i=k/1280;j=k%1280;
//fi.x+=j;fi.y+=i;
/*	for(i=0;i<7;i++){
sprintf(st,"%9.3f %9.3f %9.3f %9.3f %9.3f %9.3f",
		XX[i*6+0],
		XX[i*6+1],
		XX[i*6+2],
		XX[i*6+3],
		XX[i*6+4],
		XX[i*6+5]);
pDlg->m_lst.AddString(st);}
pDlg->m_lst.AddString("");
for(i=0;i<9;i++){
	sprintf(st,"%5.1f %5.1f %5.1f %5.1f %5.1f %5.1f ",XT[0][i],XT[1][i],XT[2][i],XT[3][i],XT[4][i],XT[5][i]);
pDlg->m_lst.AddString(st);
}
/*
FXYZ f[9];
f[0].x=-1;f[0].y=-1; 
f[1].x=0; f[1].y=-1;
f[2].x=1; f[2].y=-1;

f[3].x=-1;f[3].y=0; 
f[4].x=0; f[4].y=0;
f[5].x=1; f[5].y=0;

f[6].x=-1;f[6].y=1; 
f[7].x=0; f[7].y=1;
f[8].x=1; f[8].y=1;

	for(i=0;i<9;i++){l=k+ix[i];f[i].z=gv[l];}
	for(i=0;i<9;i++){
//		sprintf(st,"%5.1f %5.1f %9.5f",f[i].x,f[i].y,f[i].z);pDlg->m_lst.AddString(st);
		x=f[i].x;y=f[i].y;
		XX[0]+=x*x*x*x;
		XX[1]+=x*x*x*y;
		XX[2]+=x*x*y*y;
		XX[3]+=x*x*x;
		XX[4]+=x*x*y;
		XX[5]+=x*x;

		XX[6] +=x*y*x*x;
		XX[7] +=x*y*x*y;
		XX[8] +=x*y*y*y;
		XX[9] +=x*y*x;
		XX[10]+=x*y*y;
		XX[11]+=x*y;

		XX[12]+=y*y*x*x;
		XX[13]+=y*y*x*y;
		XX[14]+=y*y*y*y;
		XX[15]+=y*y*x;
		XX[16]+=y*y*y;
		XX[17]+=y*y;

		XX[18]+=x*x*x;
		XX[19]+=x*x*y;
		XX[20]+=x*y*y;
		XX[21]+=x*x;
		XX[22]+=x*y;
		XX[23]+=x;

		XX[24]+=y*x*x;
		XX[25]+=y*x*y;
		XX[26]+=y*y*y;
		XX[27]+=y*x;
		XX[28]+=y*y;
		XX[29]+=y;

		XX[30]+=x*x;
		XX[31]+=x*y;
		XX[32]+=y*y;
		XX[33]+=x;
		XX[34]+=y;
		XX[35]+=1;
	}
	fp=fopen("DAT\\XX.dat","wb");
	fwrite(XX,4,42,fp);fclose(fp);
	for(i=0;i<6;i++){
sprintf(st,"%9.3f %9.3f %9.3f %9.3f %9.3f %9.3f",
		XX[i*6+0],
		XX[i*6+1],
		XX[i*6+2],
		XX[i*6+3],
		XX[i*6+4],
		XX[i*6+5]);
pDlg->m_lst.AddString(st);
	}
//for(i=0;i<9;i++){sprintf(st,"%5.1f %5.1f %9.5f",f[i].x,f[i].y,f[i].z);pDlg->m_lst.AddString(st);}
for(i=0;i<9;i++){
	x=f[i].x;y=f[i].y;
	XT[0][i]=x*x;
	XT[1][i]=x*y;
	XT[2][i]=y*y;
	XT[3][i]=x;
	XT[4][i]=y;
	XT[5][i]=1;
}

for(i=0;i<9;i++){
	sprintf(st,"%5.1f %5.1f %5.1f %5.1f %5.1f %5.1f ",XT[0][i],XT[1][i],XT[2][i],XT[3][i],XT[4][i],XT[5][i]);
pDlg->m_lst.AddString(st);
}
	fp=fopen("DAT\\XT.dat","wb");
	fwrite(XT,4,56,fp);fclose(fp);
*/


	return fi;
}

/*Reference
	float XX[12];
	FillMemory(XX,sizeof(float)*12,0);
	for(k=0;k<n;k++)
	{
		x=dot[k].x;y=dot[k].y;z=x*x+y*y;
		XX[0]+=x*x;XX[4]+=y*y;XX[8]+=1;
		XX[1]+=x*y;XX[2]+=x;XX[5]+=y;
		XX[9]+=x*z;XX[10]+=y*z;XX[11]+=z;
	}
	XX[3]=XX[1];XX[6]=XX[2];XX[7]=XX[5];
	ret=all_gauss(3,XX,&XX[9]);
  */
/*
a=((x1-x2)(y3-y1)-(x3-x1)(y1-y2))/((x1-x2)(x2-x3)(x3-x1))
b=(y1-y2-a(x1*x1-x2*x2))/(x1-x2)
c=y1-a*x1*x1-b*x1

a=((x0-x1)(y2-y0)-(x2-x0)(y0-y1))/((x0-x1)(x1-x2)(x2-x0))
b=(y0-y1-a(x0*x0-x1*x1))/(x0-x1)
c=y0-a*x0*x0-b*x0

a=((-1-0)(y2-y0)-(1--1)(y0-y1))/((-1-0)(0-1)(1--1))
b=(y0-y1-a(-1*-1-0*0))/(-1-0)
c=y0-a*-1*-1-b*-1

a=(y2-y0)+2*(y0-y1)
b=-(y0-y1-a);
c=y0-a+b
c=y0-a*x0*x0-b*x0
c=y1-a*x1*x1-b*x1
  */

FXYZ maxFxy(float *gv,long k,float &max){
	CTmdDlg *pDlg = (CTmdDlg *)(AfxGetApp()->GetMainWnd());char st[80];
	FXYZ fi;
	float f[9];
	long l;
	FILE *fp;
//	sprintf(st,"row %9.5f %9.5f %9.5f",gv[k-1],gv[k],gv[k+1]);pDlg->m_lst.AddString(st);
//	sprintf(st,"col %9.5f %9.5f %9.5f",gv[k-W2],gv[k],gv[k+W2]);pDlg->m_lst.AddString(st);
	float y0,y1,y2,a,b,c,x;
	y0=gv[k-1];y1=gv[k];y2=gv[k+1];
	a=(y2-y0)+2*(y0-y1);b=-(y0-y1-a);c=y1;x=-b/a;
//	sprintf(st,"abc %9.5f %9.5f %9.5f x=%9.5f",a,b,c,x);pDlg->m_lst.AddString(st);
	fi.x=x+(k%1280);
	y0=gv[k-W2];y1=gv[k];y2=gv[k+W2];
	a=(y2-y0)+2*(y0-y1);b=-(y0-y1-a);c=y1;x=-b/a;
//	sprintf(st,"abc %9.5f %9.5f %9.5f y=%9.5f",a,b,c,x);pDlg->m_lst.AddString(st);
	fi.y=x+k/1280;fi.z=gv[k];

	return fi;
}
void CTmdDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	char st[80];
	long l,k,cnt,lcnt,n,m;
	int i,j,k0,k1,i0,j0,i1,j1,wk;
	FILE *fp;
	BYTE *b2;
//	float *gv=(float *)new float[HEIGHT*W2];
	float sum,max,mbx,x,y,r;
	POINT a;
	FXYZ fi,fj;


	if(!(fp=fopen("DAT\\sft.dat","rb")))return;fread(&a,sizeof(POINT),1,fp);fclose(fp);	sprintf(st,"sft %3d %3d",a.x,a.y);m_lst.AddString(st);
	if(!(fp=fopen("DAT\\sbpx1.dat","rb")))
	{
		float *gv=(float *)new float[HEIGHT*W2];
		if(!(fp=fopen("DAT\\sft.dat","rb")))return;fread(&a,sizeof(POINT),1,fp);fclose(fp);	
		sprintf(st,"sft %3d %3d",a.x,a.y);m_lst.AddString(st);
		for(k0=0;k0<2;k0++)
		{sprintf(st,"DAT\\gv%d.dat",k0);
		if(!(fp=fopen(st,"rb")))return;fread(gv,4,HEIGHT*W2,fp);fclose(fp);
		F1.clear();
		for(i=1;i<HEIGHT-1;i++)for(j=1;j<W2-1;j++){
			k=i*W2+j;
			if(maxLcl(gv,k,max)){
			fi= maxFxy(gv,k,max);
			F1.push_back(fi);
			}
		}
		sprintf(st,"size=%d",F1.size());m_lst.AddString(st);
		sprintf(st,"DAT\\sbpx%d.dat",k0);fp=fopen(st,"wb");for(k=0;k<F1.size();k++)fwrite(&F1[k],12,1,fp);fclose(fp);
		}
		delete [] gv;
		return;
	}

//===================================

/*	for(k0=0;k0<2;k0++){
		FillMemory(b0,HEIGHT*W2,0);
		sprintf(st,"DAT\\sbpx%d.dat",k0);
		if(!(fp=fopen(st,"rb")))return;
		fseek(fp,0,SEEK_END);l=ftell(fp);n=l/12;fseek(fp,0,SEEK_SET);
		sprintf(st,"l=%d n=%d",l,n);m_lst.AddString(st);
		FXYZ *spx=(FXYZ *)new FXYZ[n];
		fread(spx,12,n,fp);fclose(fp);
		cnt=0;
		for(k=0;k<n;k++){
			if(spx[k].z>0.5)
			{
				cnt++;
				i=spx[k].y;
				j=spx[k].x;
				b0[i*W2+j]=255;
			}
		}
		sprintf(st,"cnt=%d",cnt);m_lst.AddString(st);
		dsply();Sleep(2000);
		delete [] spx;
	}
return;*/
//==========================================
	FXYZ cell[72][128]={0};
	if(!(fp=fopen("DAT\\src.dat","rb")))
	{
		if(!(fp=fopen("DAT\\sbpx0.dat","rb")))return;
		fseek(fp,0,SEEK_END);l=ftell(fp);n=l/12;fseek(fp,0,SEEK_SET);
		sprintf(st,"l=%d n=%d",l,n);m_lst.AddString(st);
		FXYZ *spx=(FXYZ *)new FXYZ[n];
		fread(spx,12,n,fp);fclose(fp);
//		FillMemory(b0,HEIGHT*W2,0);for(k=0;k<n;k++){i=spx[k].y;j=spx[k].x;b0[i*W2+j]=255;}dsply();
		for(k=0;k<n;k++){
			i=spx[k].y/10;;j=spx[k].x/10;
			if(spx[k].z>cell[i][j].z)cell[i][j]=spx[k];
		}
		cnt=0;F1.clear();
		FillMemory(b0,HEIGHT*W2,0);
		for(i0=0;i0<72;i0++)for(j0=0;j0<128;j0++){
			i=cell[i0][j0].y;
			j=cell[i0][j0].x;
			if(cell[i0][j0].z>0.1){F1.push_back(cell[i0][j0]);
				b0[i*W2+j]=255;cnt++;
			}
		}
		dsply();sprintf(st,"cnt=%d ",cnt);m_lst.AddString(st);
		fp=fopen("DAT\\src.dat","wb");
		for(k=0;k<F1.size();k++)fwrite(&F1[k],12,1,fp);fclose(fp);
		for(k=0;k<F1.size();k+=10){sprintf(st,"%04d %9.3f %9.3f %9.5f",k,F1[k].x,F1[k].y,F1[k].z);m_lst.AddString(st);}
				
//m_lst.AddString("here");

		delete [] spx;
	
//		return;
	}
//==================================
	F2XY f2x;
	if(!(fp=fopen("DAT\\src.dat","rb")))return;
	fseek(fp,0,SEEK_END);l=ftell(fp);n=l/12;fseek(fp,0,SEEK_SET);
	sprintf(st,"l=%d n=%d",l,n);m_lst.AddString(st);
	FXYZ *spx=(FXYZ *)new FXYZ[n];
	fread(spx,12,n,fp);fclose(fp);

	if(!(fp=fopen("DAT\\sbpx1.dat","rb")))return;
	fseek(fp,0,SEEK_END);l=ftell(fp);m=l/12;fseek(fp,0,SEEK_SET);
	sprintf(st,"A l=%d m=%d",l,m);m_lst.AddString(st);
	FXYZ *sqx=(FXYZ *)new FXYZ[m];
	fread(sqx,12,m,fp);fclose(fp);

	FillMemory(b0,HEIGHT*W2,0);cnt=0;for(k=0;k<n;k++){i=spx[k].y;j=spx[k].x;b0[i*W2+j]=255;cnt++;}dsply();sprintf(st,"cnt=%d",cnt);m_lst.AddString(st);
//	sprintf(st,"sft x=%d y=%d",a.x,a.y);m_lst.AddString(st);
	F2.clear();cnt=0;
	for(k=0;k<n;k++){
//		sprintf(st,"%04d %9.3f %9.3f %9.5f",k,spx[k].x,spx[k].y,spx[k].z);m_lst.AddString(st);
		fi=spx[k];fi.x+=a.x;fi.y+=a.y;
		F1.clear();
		for(l=0;l<m;l++){
			fj=sqx[l];
			x=fj.x-fi.x;y=fj.y-fi.y;r=sqrt(x*x+y*y);
			if(r>8.0)continue;
			fj.z=fabs(fj.z-fi.z);
//		sprintf(st,"f %9.6f %9.6f",fi.z,fj.z);m_lst.AddString(st);
			F1.push_back(fj);cnt++;
		}
		if(F1.size()==0)continue;
//		for(i=0;i<F1.size();i++){sprintf(st,"size=%d %9.3f %9.3f %9.6f",F1.size(),F1[i].x,F1[i].y,F1[i].z);m_lst.AddString(st);}
		FXYZ *f=(FXYZ *)new FXYZ[F1.size()];
//for(i;i<F1.size();i++){sprintf(st,"%d %9.5f",i,F1[i].z);m_lst.AddString(st);}
;		for(i=0;i<F1.size();i++)f[i]=F1[i];
		Sort7z(f,F1.size(),0);
//		for(i=0;i<F1.size();i++){sprintf(st,"size=%d %9.3f %9.3f %9.6f",F1.size(),f[i].x,f[i].y,f[i].z);m_lst.AddString(st);}

		f2x.x0=spx[k].x;f2x.y0=spx[k].y;
		f2x.x1=f[0].x;f2x.y1=f[0].y;
		delete [] f;
//		sprintf(st,"f2x %9.3f %9.3f |%9.3f %9.3f ",f2x.x0,f2x.y0,f2x.x1,f2x.y1);m_lst.AddString(st);
		F2.push_back(f2x);

//		sprintf(st,"%04d size=%d",k,F1.size());m_lst.AddString(st);
	}
	sprintf(st,"cnt %04d %04d",cnt,F2.size());m_lst.AddString(st);
	for(k=0;k<F2.size();k++){
		f2x=F2[k];
//		sprintf(st,"%04d %9.3f %9.3f|%9.3f %9.3f",k,f2x.x0,f2x.y0,f2x.x1,f2x.x1);m_lst.AddString(st);
	}
	fp=fopen("DAT\\mtchF2xy.dat","wb");
	for(k=0;k<F2.size();k++)fwrite(&F2[k],sizeof(F2XY),1,fp);fclose(fp);

	delete [] spx,sqx;

	m_lst.AddString("");m_lst.AddString("finished");



}


void CTmdDlg::OnButton5() 
{
	// TODO: Add your control notification handler code here
	char st[80];
	long l,k,cnt,lcnt,n;;
	int i,j,k0,k1,i0,j0,i1,j1,wk;
	FILE *fp,*fq;
	FXYZ fi,fj,tt;
	float x,y,z,r,la,lb,La,Lb,lc,af,bt,gm;
	if(!(fp=fopen("DAT\\tt.dat","rb")))return;
	fread(&tt,12,1,fp);fclose(fp);
	tt=guiy1(tt,lc);
	sprintf(st,"tt %9.6f %9.6f %9.6f %9.3f",tt.x,tt.y,tt.z,lc);m_lst.AddString(st);
	if(!(fp=fopen("DAT\\mtchF2xy.dat","rb")))return;
	fseek(fp,0,SEEK_END);l=ftell(fp);n=l/16;
	fseek(fp,0,SEEK_SET);
	F2XY *mh=(F2XY *)new F2XY[n];
	fread(mh,16,n,fp);fclose(fp);
	sprintf(st,"l=%d n=%d",l,n);m_lst.AddString(st);
	fp=fopen("DAT\\cloud0.dat","wb");
	fq=fopen("DAT\\cloud1.dat","wb");	
//	for(k=0;k<n;k++){sprintf(st,"%03d %9.3f %9.3f | %9.3f %9.3f ",k,mh[k].x0,mh[k].y0,mh[k].x1,mh[k].y1);m_lst.AddString(st);}
	for(k=0;k<n;k++){
		fi.x=mh[k].x0;fi.y=mh[k].y0;
		fi=pdt2vct(fi,la);
		fj.x=mh[k].x1;fj.y=mh[k].y1;
		fj=pdt2vct(fj,lb);
//		sprintf(st,"%05d %9.6f %9.6f %9.6f |%9.3f",k,fi.x,fi.y,fi.z,la);m_lst.AddString(st);
//		sprintf(st,"%05d %9.6f %9.6f %9.6f |%9.3f",k,fj.x,fj.y,fj.z,lb);m_lst.AddString(st);

		gm=acos(fi.x*fj.x+fi.y*fj.y+fi.z*fj.z);
		af=acos(fi.x*tt.x+fi.y*tt.y+fi.z*tt.z);
		bt=pi-af-gm;
//		sprintf(st,"%05d %9.4f %9.4f %9.4f",k,af*180/pi,bt*180/pi,gm*180/pi);m_lst.AddString(st);
		La=sin(bt)*lc/sin(gm);
		Lb=sin(af)*lc/sin(gm);
		fi.x*=La;fi.y*=La;fi.z*=La;
		fj.x*=Lb;fj.y*=Lb;fj.z*=Lb;
//		sprintf(st,"%05d %9.3f %9.3f %9.3f |%9.3f",k,fi.x,fi.y,fi.z,La);m_lst.AddString(st);
//		sprintf(st,"%05d %9.3f %9.3f %9.3f |%9.3f",k,fj.x,fj.y,fj.z,Lb);m_lst.AddString(st);			
		fwrite(&fi,12,1,fp);
		fwrite(&fj,12,1,fq);
	}
//=====================================

	float Q[16],A[9];
	if(!(fp=fopen("DAT\\cloud0.dat","rb")))return;
	fseek(fp,0,SEEK_END);l=ftell(fp);n=l/12;fseek(fp,0,SEEK_SET);
	sprintf(st,"l=%d n=%d",l,n);m_lst.AddString(st);
	FXYZ *cld=(FXYZ *)new FXYZ[n];
	fread(cld,12,n,fp);fclose(fp);
	cld2eig(cld,n,Q,A);
	m_lst.AddString("Q:");for(i=0;i<4;i++){sprintf(st,"%9.6f %9.6f %9.6f %9.3f",Q[i*4+0],Q[i*4+1],Q[i*4+2],Q[i*4+3]);m_lst.AddString(st);}
	m_lst.AddString("A:");sprintf(st,"%9.3f %9.3f %9.6f",A[0],A[4],A[8]);m_lst.AddString(st);
	inv4(Q);
	m_lst.AddString("Q:");for(i=0;i<4;i++){sprintf(st,"%9.6f %9.6f %9.6f %9.3f",Q[i*4+0],Q[i*4+1],Q[i*4+2],Q[i*4+3]);m_lst.AddString(st);}
	for(k=0;k<n;k++)cld[k]=mvct4a(Q,cld[k]);
	fp=fopen("DAT\\cld0.dat","wb");
	fwrite(cld,12,n,fp);fclose(fp);
//	for(k=0;k<n;k+=10){fi=cld[k];sprintf(st,"%05d %9.3f %9.3f %9.3f ",k,fi.x,fi.y,fi.z);m_lst.AddString(st);}

	m_lst.AddString("here");



	m_lst.AddString("");
	m_lst.AddString("Finished");
}


void CTmdDlg::OnButton6() 
{
	SetTimer(1,200,NULL);
	return;
	static int cnt=0;
	long isum,jsum;
	int i0,j0,k1,wk,kcnt;
	FXYZ fi,*b1;
	FILE *fp;
	long k,l,n;
	int i,j,k0;
	char st[80];
	float rate=0.5,gm;
	float Q[16];


/*	BLOCKINFO blk;
	BYTE *bw=(BYTE *)new BYTE [720*1024];
	FillMemory(&blk,sizeof(blk),0);
	blk.iBitCount=8;
	blk.iFormType=FORM_GRAY8;
	blk.iHeight=720;
	blk.iWidth=1024;
	blk.lpBits = bw;
	for(k=0;k<720*1024;k++)bw[k]=k%256;
	okSaveImageFile(hBrd,"JPG\\TW.jpg",70,TARGET(&blk),0,1);
	okConvertRect(hBrd,SCREEN,0,TARGET(&blk),0,1);*/
//	okLoadImageFile(hBrd,"JPG\\TW.jpg",0,BUFFER,0,1);
	sprintf(st,"cnt=%3d",cnt);SetWindowText(st);
	FillMemory(b0,HEIGHT*W2,0);
	for(i=0;i<HEIGHT;i++)b0[i*W2+W4]=127;
	for(j=0;j<W2;j++)b0[H2*W2+j]=127;


	if(!(fp=fopen("DAT\\cld0.dat","rb")))return;
	fseek(fp,0,SEEK_END);l=ftell(fp);n=l/12;fseek(fp,0,SEEK_SET);

	fread(cld,12,n,fp);fclose(fp);
	for(k=0;k<n;k++){
		i=cld[k].y+H2;
		j=cld[k].x+W4;
		b0[i*W2+j]=255;
	}

	okSaveImageFile(hBrd,"JPG\\cld0.jpg",70,BUFFER,0,1);
	dsply();

	cnt++;cnt%=360;
	gm=cnt*pi/3600;
	FillMemory(Q,64,0);
	Q[0]=Q[10]=cos(gm);Q[2]=sin(gm);Q[8]=-Q[2];Q[5]=Q[15]=1;
	for(k=0;k<n;k++)cld[k]=mvct4a(Q,cld[k]);

	fp=fopen("DAT\\cld0.dat","wb");
	fwrite(cld,12,n,fp);fclose(fp);

}

void CTmdDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	char st[80];
	int i0,j0,i,j;
//	PNT p;

	i0=point.y*3/4;j0=point.x*3/2;
	sprintf(st,"x=%4d y=%4d | j0=%4d i0=%4d",point.x,point.y,j0,i0);m_lst.AddString(st);
//	for(i=-5;i<=5;i++)for(j=-5;j<=5;j++)pnt[(i+i0)*W2+j+j0]=RED;
//	p=pnt[i0*W2+j0];
//	for(j=j0;j<W2;j++)for(i=0;i<HEIGHT;i++)pnt[i*W2+j]=p;
//	dsply();
//	sprintf(st,"BMP\\fs%d.bmp",m_cmb.GetCurSel());m_lst.AddString (st);
//	okSaveImageFile(hBrd,st,100,BUFFER,0,1);




/*
	char st[80];
	static int cnt=0;
	static CPoint zn,cr; 
	POINT b,e;
	float sum,lcnt;
	int i,j;
	cnt%=2;
	cr.y=point.y*3/2;cr.x=point.x*3/2;
	sprintf(st,"x=%d y=%d",cr.x,cr.y);SetWindowText(st);
	if(cnt==0){zn=cr;cnt++;return;}


	rct0.left=zn.x;  rct0.top=zn.y;
	rct0.right=cr.x; rct0.bottom =cr.y;

//	sprintf(st,"%4d %4d %4d %4d",rct0.left ,rct0.top ,rct0.right ,rct0.bottom );m_lst.AddString(st);

	b.x=rct0.left ;b.y=rct0.top ;
	e.x=rct0.right ;e.y=rct0.top ;
	dda(b,e,HEIGHT,W2,b0,255);
	b.x=rct0.right ;b.y=rct0.top ;
	e.x=rct0.right  ;e.y=rct0.bottom ;
	dda(b,e,HEIGHT,W2,b0,255);
	b.x=rct0.right  ;b.y=rct0.bottom ;
	e.x=rct0.left  ;e.y=rct0.bottom ;
	dda(b,e,HEIGHT,W2,b0,255);
	b.x=rct0.left   ;b.y=rct0.top ;
	e.x=rct0.left   ;e.y=rct0.bottom  ;
	dda(b,e,HEIGHT,W2,b0,255);
	okConvertRect(hBrd,SCREEN,0,BUFFER,0,1);
	sum=0;lcnt=0;
	for(i=rct0.top;i<rct0.bottom ;i++)for(j=rct0.left ;j<rct0.right ;j++){sum+=b0[i*W2+j];lcnt++;}
	gray=sum/lcnt;
		sprintf(st,"gray=%d",gray);m_lst.AddString(st);;
	cnt++;
*/
	CDialog::OnRButtonUp(nFlags, point);
}

FXYZ stdd3(float *a,int n) 
{
	FXYZ ret;
	float sum=0,ave,x,sg;
	int k;
	sum=0;for(k=0;k<n;k++)sum+=a[k];ave=sum/n;
	sum=0;for(k=0;k<n;k++){x=a[k]-ave;sum+=x*x;}
	sg=sqrt(sum/n);
	ret.x=ave;ret.y=sg;ret.z=n;
	return ret;
}
void Sort7z(FXYZ arr[],long cnt,BYTE flg){
	int smallNo;  
	int pass,j;     
	FXYZ temp; 
	if(flg==0){
	for (pass=0;pass<cnt-1;pass++){ 
		smallNo=pass; 
		for(j=pass+1;j<cnt;j++) 
			if(arr[j].z<arr[smallNo].z)smallNo=j; 
		if(smallNo!=pass){ 
			temp=arr[pass]; 
			arr[pass]=arr[smallNo]; 
			arr[smallNo]=temp; 
		} 
	} 
	}
	else{
	for (pass=0;pass<cnt-1;pass++){ 
		smallNo=pass; 
		for(j=pass+1;j<cnt;j++) 
			if(arr[j].z>arr[smallNo].z)smallNo=j; 
		if(smallNo!=pass){ 
			temp=arr[pass]; 
			arr[pass]=arr[smallNo]; 
			arr[smallNo]=temp; 
		} 
	} 
	}
}

void CTmdDlg::OnChangeEdit1() 
{
	UpdateData(true);	
}


void genXX(FXYZ *xy,long n,float *XX)
{
	long k;
	float xx,Xy,yy,x,y,z;
	FillMemory(XX,sizeof(float)*30,0);
	for(k=0;k<n;k++)	
	{
		if(xy[k].x==0.&&xy[k].y==0.)continue;
		{
		xx=xy[k].x*xy[k].x;
		Xy=xy[k].x*xy[k].y;
		yy=xy[k].y*xy[k].y;
		x=xy[k].x;
		y=xy[k].y;
		z=-xx;

		XX[0]+=yy*yy;
		XX[6]+=Xy*Xy;
		XX[12]+=xx;
		XX[18]+=yy;
		XX[24]+=1;

		XX[1]+=yy*Xy;
		XX[2]+=yy*x;
		XX[3]+=yy*y;
		XX[4]+=yy;

		XX[7]+=Xy*x;
		XX[8]+=Xy*y;
		XX[9]+=Xy;

		XX[13]+=Xy;
		XX[14]+=x;

		XX[19]+=y;

		XX[25]+=yy*z;
		XX[26]+=Xy*z;
		XX[27]+=x*z;
		XX[28]+=y*z;	
		XX[29]+=z;	
		}
	}

//对称
	XX[5]=XX[1];

	XX[10]=XX[2];
	XX[11]=XX[7];

	XX[15]=XX[3];
	XX[16]=XX[8];
	XX[17]=XX[13];

	XX[20]=XX[4];
	XX[21]=XX[9];
	XX[22]=XX[14];
	XX[23]=XX[19];
}

ELPS fitElp(FXYZ *xy,long n)
{
	FXYZ zr={0.,0.,0.};FXY p;
//	char st[80];
	ELPS e;
	float *XX=(float *)new float[30];
	long k,kcnt;
	float U,V,W,X,Y,Z,Q,C2,C4,xp,xm;
	float xc,yc,a,b,bt,A,B,C,sume,w0,w3,x,y,r;
	FXYZ xy0;
	int ret;
	CTmdDlg *pDlg = (CTmdDlg *)(AfxGetApp()->GetMainWnd());char st[80];
	xy0=xy[0];
	genXX(xy,n,XX);
//	for(i=0;i<6;i++){sprintf(st,"%16.3f %16.3f %16.3f %16.3f %16.3f",XX[i*5],XX[i*5+1],XX[i*5+2],XX[i*5+3],XX[i*5+4]);pDlg->m_lst.AddString(st);}
	ret=all_gauss(5,XX,&XX[25]);
	if(ret==0){/*AfxMessageBox("all_gauss fail!");*/FillMemory(&e,sizeof(ELPS),0);return e;}

//pDlg->m_lst.AddString("");	sprintf(st,"%16.3f %16.3f %16.3f %16.3f %16.3f",XX[25],XX[26],XX[27],XX[28],XX[29]);pDlg->m_lst.AddString(st);
//pDlg->m_lst.AddString("");

	yc=(2*XX[28]/XX[26]-XX[27])/(XX[26]-4*XX[25]/XX[26]);	//圆心
	xc=(-XX[26]*yc-XX[27])/2.;e.c.x=xc;e.c.y=yc;			//圆心

	Q=(1.-XX[25])/XX[26];

	C=Q+sqrt(Q*Q+1.);
	bt=atan(C);

	e.bt=bt;										//倾角

	U=1./(xc*xc+XX[25]*yc*yc+XX[26]*xc*yc-XX[29]);
	V=XX[25]*U;
	W=XX[26]*U;
	X=XX[27]*U;
	Y=XX[28]*U;
	Z=XX[29]*U-1;

//pDlg->m_lst.AddString("");
//	sprintf(st,"%15.12f %15.12f %15.12f",U,V,W);pDlg->m_lst.AddString(st);
//	sprintf(st,"%15.12f %15.12f %15.12f",X,Y,Z);pDlg->m_lst.AddString(st);
//pDlg->m_lst.AddString("");
	C2=C*C;
	C4=C2*C2;
	A=sqrt((U-C2*V)/(1.-C4));
	B=sqrt((V-C2*U)/(1.-C4));

	a=cos(bt)/A;e.a=a;										//横轴
	b=cos(bt)/B;e.b=b;										//竖轴


//==================================
//==================================
//(b(x-xc))^2+(a(y-yc))^2-(ab)^2=0  
//(a(y-yc))^2=(ab)^2-(b(x-xc))^2
//x=xc+sqrt(b^2-(y-yc)^2)*a/b

/*	sume=0.;kcnt=0;		//计算均方差
	for(k=0;k<n;k++)	
	{
		if(xy[k].x==0.&&xy[k].y==0.)continue;
		p.x=(xy[k].x-xc)*cos(bt)-(xy[k].y-yc)*sin(bt);
		p.y=(xy[k].x-xc)*sin(bt)+(xy[k].y-yc)*cos(bt);
		w0=(b+p.y)*(b-p.y);
		if(w0<0.)continue;
		xp=+sqrt(w0)*a/b;
		xm=-sqrt(w0)*a/b;	
		if(fabs(p.x-xp)<fabs(p.x-xm))xy[k].z=w3=p.x-xp;else xy[k].z=w3=p.x-xm;
		sume+=(w3*w3);kcnt++;		
	}
	sume/=kcnt;
	e.me=sqrt(sume);
	for(k=0;k<n;k++)if(fabs(xy[k].z)>1.*e.me)xy[k]=zr;	*/

	sume=0.;kcnt=0;
	for(k=0;k<n;k++)	
	{
		if(xy[k].x==0.&&xy[k].y==0.)continue;
		p.x=(xy[k].x-xc)*cos(bt)-(xy[k].y-yc)*sin(bt);
		p.y=(xy[k].x-xc)*sin(bt)+(xy[k].y-yc)*cos(bt);
		w0=p.x*p.x/(a*a)+p.y*p.y/(b*b)-1.;
		xy[k].z=w0=fabs(w0);
		sume+=w0;kcnt++;	
	}
	sume/=kcnt;
	e.me=sqrt(sume);
//	for(k=0;k<n;k++)if(fabs(xy[k].z)>0.06)xy[k]=zr;   //计算点到椭圆圆周的距离，并过滤****过滤参数1
	for(k=0;k<n;k++)if(fabs(xy[k].z)>0.03)xy[k]=zr;    //计算点到椭圆圆周的距离，并过滤****过滤参数1

//======================================	
	genXX(xy,n,XX);
//	for(i=0;i<6;i++){sprintf(st,"%16.3f %16.3f %16.3f %16.3f %16.3f",XX[i*5],XX[i*5+1],XX[i*5+2],XX[i*5+3],XX[i*5+4]);pDlg->m_lst.AddString(st);}
	ret=all_gauss(5,XX,&XX[25]);
	if(ret==0){/*AfxMessageBox("all_gauss fail!");*/FillMemory(&e,sizeof(ELPS),0);return e;}

//pDlg->m_lst.AddString("");	sprintf(st,"%16.3f %16.3f %16.3f %16.3f %16.3f",XX[25],XX[26],XX[27],XX[28],XX[29]);pDlg->m_lst.AddString(st);
//pDlg->m_lst.AddString("");

	yc=(2*XX[28]/XX[26]-XX[27])/(XX[26]-4*XX[25]/XX[26]);	//圆心
	xc=(-XX[26]*yc-XX[27])/2.;e.c.x=xc;e.c.y=yc;			//圆心
	
	Q=(1.-XX[25])/XX[26];
	C=Q+sqrt(Q*Q+1.);
	bt=atan(C);

 //	if(bt>pi/4.)bt-=pi/2.;

	e.bt=bt;										//倾角??????????

	U=1./(xc*xc+XX[25]*yc*yc+XX[26]*xc*yc-XX[29]);
	V=XX[25]*U;
	W=XX[26]*U;
	X=XX[27]*U;
	Y=XX[28]*U;
	Z=XX[29]*U-1;

//pDlg->m_lst.AddString("");
//	sprintf(st,"%15.12f %15.12f %15.12f",U,V,W);pDlg->m_lst.AddString(st);
//	sprintf(st,"%15.12f %15.12f %15.12f",X,Y,Z);pDlg->m_lst.AddString(st);
//pDlg->m_lst.AddString("");

	C2=C*C;
	C4=C2*C2;
	A=sqrt((U-C2*V)/(1.-C4));
	B=sqrt((V-C2*U)/(1.-C4));

	a=cos(bt)/A;e.a=a;										//横轴
	b=cos(bt)/B;e.b=b;										//竖轴
	if(a<b){e.a=b;e.b=a;e.bt-=pi/2;}
	
	x=xy0.x-e.c.x;y=xy0.y-e.c.y;r=sqrt(x*x+y*y);
	w0=asin(y/r);if(x<0)w0=pi-w0;
	e.ag=w0;
//	sprintf(st,"xy0 %9.3f %9.3f %9.4f°",xy0.x,xy0.y,w0*180/pi);pDlg->m_lst.AddString(st);
//	sprintf(st,"xyr %9.3f %9.3f %9.3f",x,y,r);pDlg->m_lst.AddString(st);
//	sprintf(st,"e.c %9.3f %9.3f",e.c.x,e.c.y);pDlg->m_lst.AddString(st);

//================================
	sume=0.;kcnt=0;		//计算均方差
	for(k=0;k<n;k++)	
	{
		if(xy[k].x==0.&&xy[k].y==0.)continue;
		p.x=(xy[k].x-xc)*cos(bt)-(xy[k].y-yc)*sin(bt);
		p.y=(xy[k].x-xc)*sin(bt)+(xy[k].y-yc)*cos(bt);
		w0=(b+p.y)*(b-p.y);
		if(w0<0.)continue;
		xp=+sqrt(w0)*a/b;
		xm=-sqrt(w0)*a/b;	
		if(fabs(p.x-xp)<fabs(p.x-xm))xy[k].z=w3=p.x-xp;else xy[k].z=w3=p.x-xm;
		sume+=(w3*w3);kcnt++;		
	}
	sume/=kcnt;
	e.me=sqrt(sume);
//	for(k=0;k<n;k++)if(fabs(xy[k].z)>1.*e.me)xy[k]=zr;	       //计算均方差，并过滤****过滤参数2

//	if(e.bt>1.)e.bt=fabs(e.bt-(pi/2.));
//	if(e.bt>pi/4.)e.bt=fabs(e.bt-(pi/2.));

	delete [] XX;

	return e;
}

float tsFx(float a,FXYZ t1,FXYZ c0)
{
	CTmdDlg *pDlg = (CTmdDlg *)(AfxGetApp()->GetMainWnd());//char st[80];
	float x,y,z,r;
	x=a*t1.x-c0.x;
	y=a*t1.y-c0.y;
	z=a*t1.z-c0.z;;
	r=sqrt(x*x+y*y+z*z);
//	sprintf(st,"xyz %9.3f %9.3f %9.3f %9.3f",x,y,z,r-RADIUS);pDlg->m_lst.AddString(st);
//	sprintf(st,"c0 %9.3f %9.3f %9.3f",c0.x,c0.y,c0.z);pDlg->m_lst.AddString(st);
//	sprintf(st,"t1 %9.6f %9.6f %9.6f",t1.x,t1.y,t1.z);pDlg->m_lst.AddString(st);
	return r-RADIUS;
}
float tsFind(float a,float b,FXYZ t1,FXYZ c0)
{
	CTmdDlg *pDlg = (CTmdDlg *)(AfxGetApp()->GetMainWnd());//char st[80];
	float lb,fa,fb,fm,m;
	m=(a+b)/2;
//	sprintf(st,"a=%9.3f b=%9.3f",a,b);pDlg->m_lst.AddString(st);
	fa=tsFx(a,t1,c0);
	fb=tsFx(b,t1,c0);
	if(fa*fb>0){pDlg->m_lst.AddString("fa*fb>0!");return 0;}
	fm=tsFx(m,t1,c0);
	while(fabs(a-b)>0.0001){
		m=(a+b)/2;
		if(fa*fm>0){fa=fm;a=m;}else{fb=fm;b=m;}
		fm=tsFx(m,t1,c0);
//		sprintf(st,"%9.3f %9.3f %9.3f|%9.3f %9.3f %9.3f",a,b,m,fa,fb,fm);pDlg->m_lst.AddString(st);
//		sprintf(st,"fa=%9.3f fb=%9.3f fm=%9.3f",fa,fb,fm);pDlg->m_lst.AddString(st);
	}
	lb=a;
	return lb;
}

void CTmdDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	return;
	char st[80];
	int i0,j0,i,j,k0,k1;;
	CPoint lt;
	float sum,isum,jsum;
	FXYZ fi;
	i0=point.y*3/2;j0=point.x*3/2;
//	sprintf(st,"i0=%4d j0=%4d",i0,j0);m_lst.AddString(st);
	lt.x=rct0.left =j0-THRT;rct0.right =j0+THRT;
	lt.y=rct0.top =i0-THRT;rct0.bottom =i0+THRT;	
//	sprintf(st,"%4d %4d %4d %4d",rct0.left,rct0.top,rct0.right,rct0.bottom);m_lst.AddString(st);
	FILE *fp;
	if(!(fp=fopen("DAT\\dt.dat","rb"))){m_lst.AddString("data not found!");return;}
	fread(dt,12,150,fp);fclose(fp);
	k0=m_cmb.GetCurSel();
	k1=m_dot.GetCurSel();
	BLOCKINFO blk;
	BYTE *bf=(BYTE *)new BYTE [THR4];
	BYTE *bw=(BYTE *)new BYTE [THR4];
	FillMemory(&blk,sizeof(blk),0);
	blk.iBitCount=8;
	blk.iFormType=FORM_GRAY8;
	blk.iHeight=THR2;
	blk.iWidth=THR2;
	blk.lpBits = bw;
	for(i=0;i<THR2;i++)for(j=0;j<THR2;j++){
		bf[i*THR2+j]=b0[(lt.y+i)*W2+lt.x+j];
	}
	im2bw(bf,bw,THR2,THR2);
	sum=isum=jsum=0;
	for(i=0;i<THR2;i++)for(j=0;j<THR2;j++){if(bw[i*THR2+j]>127){sum++;isum+=i;jsum+=j;}}
	isum/=sum;jsum/=sum;
	i0=isum;j0=jsum;
	for(i=-1;i<=1;i++)for(j=-1;j<=1;j++)bw[(i0+i)*THR2+j0+j]=0;

	RECT rct;SetRect(&rct,2,40,2+THR2,40+THR2);
	okSetTargetRect(hBrd,SCREEN,&rct);	
	okConvertRect(hBrd,SCREEN,0,TARGET(&blk),0,1);
	delete [] bf,bw;
	dt[k0][k1].x=lt.x+jsum;
	dt[k0][k1].y=lt.y+isum;
	fi=dt[k0][k1];
	sprintf(st,"%d %02d | %9.3f,%9.3f",k0,k1,fi.x,fi.y);m_lst.AddString(st);

	i0=fi.y;j0=fi.x;

	for(i=-2;i<=2;i++)for(j=-2;j<=2;j++)b0[(i+i0)*W2+j+j0]=0;
	dsply();

	k1=m_dot.GetCurSel();
	k1++;k1%=25;m_dot.SetCurSel(k1);
	if(k1==0){
		k0=m_cmb.GetCurSel();
		k0++;k0%=6;m_cmb.SetCurSel(k0);
	}
	fp=fopen("DAT\\dt.dat","wb");fwrite(dt,12,150,fp);fclose(fp);




	CDialog::OnLButtonDblClk(nFlags, point);
}
void dsply(){
	RECT rct;
	SetRect(&rct,2,40,2+W2,40+HEIGHT);
//	SetRect(&rct,0,0,W2,HEIGHT);
	okSetTargetRect(hBrd,SCREEN,&rct);
	okConvertRect(hBrd,SCREEN,0,BUFFER,0,1);
}

void CTmdDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	static long TCnt=0;
//	char st[80];
//	sprintf(st,"Tcnt=%d",TCnt++);SetWindowText(st);
	static int cnt=0;
	long isum,jsum;
	int i0,j0,k1,wk,kcnt;
	FXYZ fi,*b1;
	FILE *fp;
	long k,l,n;
	int i,j,k0;
	char st[80];
	float rate=0.5,gm;
	float Q[16];


/*	BLOCKINFO blk;
	BYTE *bw=(BYTE *)new BYTE [720*1024];
	FillMemory(&blk,sizeof(blk),0);
	blk.iBitCount=8;
	blk.iFormType=FORM_GRAY8;
	blk.iHeight=720;
	blk.iWidth=1024;
	blk.lpBits = bw;
	for(k=0;k<720*1024;k++)bw[k]=k%256;
	okSaveImageFile(hBrd,"JPG\\TW.jpg",70,TARGET(&blk),0,1);
	okConvertRect(hBrd,SCREEN,0,TARGET(&blk),0,1);*/
//	okLoadImageFile(hBrd,"JPG\\TW.jpg",0,BUFFER,0,1);
//	sprintf(st,"cnt=%3d",cnt);SetWindowText(st);
	FillMemory(b0,HEIGHT*W2,0);
	for(i=0;i<HEIGHT;i++)b0[i*W2+W4]=127;
	for(j=0;j<W2;j++)b0[H2*W2+j]=127;


	if(!(fp=fopen("DAT\\cld0.dat","rb")))return;
	fseek(fp,0,SEEK_END);l=ftell(fp);n=l/12;fseek(fp,0,SEEK_SET);

	fread(cld,12,n,fp);fclose(fp);
	for(k=0;k<n;k++){
		i=cld[k].y+H2;
		j=cld[k].x+W4;
		b0[i*W2+j]=255;
	}

	okSaveImageFile(hBrd,"JPG\\cld0.jpg",70,BUFFER,0,1);
	dsply();

	cnt++;cnt%=360;
	gm=cnt*pi/3600;
	FillMemory(Q,64,0);
	Q[0]=Q[10]=cos(gm);Q[2]=sin(gm);Q[8]=-Q[2];Q[5]=Q[15]=1;
	for(k=0;k<n;k++)cld[k]=mvct4a(Q,cld[k]);

	fp=fopen("DAT\\cld0.dat","wb");
	fwrite(cld,12,n,fp);fclose(fp);

	CDialog::OnTimer(nIDEvent);
}
