#include <windows.h>		// Windows的头文件
#include "glew.h"			// 包含最新的gl.h,glu.h库
#include <stdio.h>			// 标准输入/输出库的头文件
#include <math.h>			// 数学库函数
#include "glaux.h"			// GLaux库的头文件
#include "card.h"
#include <math.h>


HGLRC           hRC=NULL;							// 窗口着色描述表句柄
HDC             hDC=NULL;							// OpenGL渲染描述表句柄
HWND            hWnd=NULL;							// 保存我们的窗口句柄
HINSTANCE       hInstance;							// 保存程序的实例
GLfloat     cl=0.140625f;
GLfloat		cr=0.859375f;
bool	keys[256];								// 保存键盘按键的数组
bool	active=TRUE;								// 窗口的活动标志，缺省为TRUE
bool	fullscreen=TRUE;							// 全屏标志缺省，缺省设定成全屏模式
GLuint	texture[55];					// 保存5个纹理标志
GLuint	loop;						// 循环变量
GLfloat		xrot;			// X 旋转量
GLfloat		yrot;			// Y 旋转量
GLfloat		zrot;			// Z 旋转量
GLuint		card;
Pile *p[8];					//p[7]是临时牌堆，在托动牌时起作用
PileRight *pr[4];					
PileLeft *pl[2];
int	mouse_x, mouse_y;      //鼠标坐标
int down_x;                //按下左键时鼠标坐标
int down_y;
int up_x;                  //松开鼠标左键时鼠标坐标
int up_y;
int pilenum;				//将鼠标坐标转化为牌堆号和卡号
int cardnum;
bool mouseDown=false;		//鼠标按下？
bool mouseRelease=false;	//鼠标按下后又松开？
bool mouseDrag=false;		//鼠标在托动牌？
bool pileDragFilm=false;	//是否正在拖动下方牌堆？
bool flipfilm=false;//是否正在播放翻牌动画
bool downFlipFilm=false;//下牌堆的翻牌动画
bool plDragFilm=false;	//左牌堆拖动动画
bool prDragFilm=false;	//右牌堆拖动动画
GLfloat    flip;		//翻牌动画的旋转角度

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);				// WndProc的定义

GLvoid BuildLists()								// 创建盒子的显示列表
{

	card=glGenLists(54);							// 创建两个显示列表的名称
	for(int i=0;i<53;i++){
	glNewList(card+i,GL_COMPILE);					// 创建第一个显示列表

		//glEnable(GL_BLEND);									// 启用混合
	glBlendFunc(GL_DST_COLOR,GL_ZERO);				// 使用黑白“掩模”混合屏幕颜色
	glBindTexture(GL_TEXTURE_2D, texture[0]);		// 选择第二个纹理
		glBegin(GL_QUADS);								// 绘制四边形
			glTexCoord2f(cl, 0.0f); glVertex3f(-0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 0.0f); glVertex3f( 0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 1.0f); glVertex3f( 0.9f,  1.25f,  0.0f);	
			glTexCoord2f(cl, 1.0f); glVertex3f(-0.9f,  1.25f,  0.0f);	
		glEnd();
	glBlendFunc(GL_ONE, GL_ONE);					// 把纹理2复制到屏幕

	glBindTexture(GL_TEXTURE_2D, texture[i+1]);		// 选择第二个纹理
		glBegin(GL_QUADS);								// 绘制四边形
			glTexCoord2f(cl, 0.0f); glVertex3f(-0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 0.0f); glVertex3f( 0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 1.0f); glVertex3f( 0.9f,  1.25f,  0.0f);	
			glTexCoord2f(cl, 1.0f); glVertex3f(-0.9f,  1.25f,  0.0f);	
		glEnd();
	//glDisable(GL_BLEND);								// 禁用混合

	//glBindTexture(GL_TEXTURE_2D, texture[53]);		// 选择第二个纹理
	//glBegin(GL_QUADS);
	//	// 后面
	//	glTexCoord2f(0.8672f, 0.0f); glVertex3f(-0.9f, -1.25f, -0.01f);	// 纹理和四边形的右下
	//	glTexCoord2f(0.8762f, 1.0f); glVertex3f(-0.9f,  1.25f, -0.01f);	// 纹理和四边形的右上
	//	glTexCoord2f(0.1328f, 1.0f); glVertex3f( 0.9f,  1.25f, -0.01f);	// 纹理和四边形的左上
	//	glTexCoord2f(0.1328f, 0.0f); glVertex3f( 0.9f, -1.25f, -0.01f);	// 纹理和四边形的左下
	//glEnd();

	glEndList();								// 第一个显示列表结束
	}
	glNewList(card+53,GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, texture[54]);		// 选择第二个纹理
		glBegin(GL_QUADS);								// 绘制四边形
			glTexCoord2f(cl, 0.0f); glVertex3f(-0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 0.0f); glVertex3f( 0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 1.0f); glVertex3f( 0.9f,  1.25f,  0.0f);	
			glTexCoord2f(cl, 1.0f); glVertex3f(-0.9f,  1.25f,  0.0f);	
		glEnd();
	glEndList();
}


AUX_RGBImageRec *LoadBMP(CHAR *Filename)				// 载入位图图象
{
	FILE *File=NULL;									// 文件句柄
	if (!Filename)										// 确保文件名已提供
	{
		return NULL;									// 如果没提供，返回 NULL
	}
	File=fopen(Filename,"r");							// 尝试打开文件
	if (File)											// 文件存在么?
	{
		fclose(File);									// 关闭句柄
		return auxDIBImageLoadA(Filename);				// 载入位图并返回指针
	}
	return NULL;										// 如果载入失败，返回 NULL
}
int LoadGLTextures()						
{

	int Status=TRUE;						
	AUX_RGBImageRec *TextureImage[55];					// 创建保存5个纹理的数据结构
	memset(TextureImage,0,sizeof(void *)*55);			// 初始化
	char *a="Data//";
	char *b=".bmp";
	for(int i=0;i<55;i++){
		char c[13];
		for(int j=0;j<6;j++)
			c[j]=a[j];
		c[6]=i/10+48;
		c[7]=i%10+48;
		for(int k=0;k<4;k++)
			c[k+8]=b[k];
		c[12]='\0';
		if(!(TextureImage[i]=LoadBMP(c)))
			Status=FALSE;
	}							
		glGenTextures(55, &texture[0]);					// 创建5个纹理

		for (loop=0; loop<55; loop++)					// 循环加载5个纹理
		{
			glBindTexture(GL_TEXTURE_2D, texture[loop]);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX, TextureImage[loop]->sizeY,
				0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
		}
	
	for (loop=0; loop<55; loop++)					
	{
		if (TextureImage[loop])					
		{
			if (TextureImage[loop]->data)			
			{
				free(TextureImage[loop]->data);		
			}
			free(TextureImage[loop]);			
		}
	}
	return Status;						
}
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)				// 重置OpenGL窗口大小
{
	if (height==0)								// 防止被零除
	{
		height=1;							// 将Height设为1
	}
	glViewport(0, 0, width, height);					// 重置当前的视口
	glMatrixMode(GL_PROJECTION);						// 选择投影矩阵
	glLoadIdentity();							// 重置投影矩阵
	// 设置视口的大小
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);						// 选择模型观察矩阵
	glLoadIdentity();							// 重置模型观察矩阵
}
void initCard(){
	bool whole[52];//仅在初始化时使用。这52个bool代表52张牌是否被用过，顺序为黑桃1~13，红桃1~13，梅花1~13，方片1~13。
	for(int i=0;i<52;i++)
		whole[i]=false;
	int used=0;//whole数组中使用过的牌的数量。同样仅在初始化使用，为了随机发牌
	srand( (unsigned)time( NULL ) );//srand()函数产生一个以当前时间开始的随机种子.应该放在for等循环语句前面 不然要很长时间等待
	for(int i=0;i<7;i++)
		p[i]=new Pile(1,i,whole,used);//初始化7个下牌堆。
	pl[0]=new PileLeft(24,whole,used);//初始化左侧牌堆。
	pl[1]=new PileLeft();
	p[7]=new Pile();
	for(int i=0;i<4;i++)
		pr[i]=new PileRight();
}

int InitGL(GLvoid)										// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())								// 调用纹理载入子例程
	{
		return FALSE;									// 如果未能载入，返回FALSE
	}

	glEnable(GL_TEXTURE_2D);							// 启用纹理映射
	glShadeModel(GL_SMOOTH);							// 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// 黑色背景
	glClearDepth(1.0f);									// 设置深度缓存
	glEnable(GL_DEPTH_TEST);							// 启用深度测试
	glDepthFunc(GL_LEQUAL);								// 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// 告诉系统对透视进行修正
	BuildLists();										// 创建显示列表
	initCard();											//发牌！初始化牌堆
	return TRUE;										// 初始化 OK
}
void transpos(int &pn,int &cn,int x,int y){
	//将鼠标坐标转化为牌堆号和卡号,注意当牌堆为空时cn也会等于-1
	if(x%182>68)
		pn=x/182;
	else
		pn=-1;
	int c=p[pn]->getConceal();
	int s=p[pn]->getShow();
	if((y>238+c*12+(s-1)*30+150)||(y<238+c*12))//是背面牌或者超出去了
		cn=-1;
	else if(y<238+c*12+(s-1)*30)
		cn=(y-238-12*c)/30+c;
	else
		cn=c+s-1;
}
bool releaseable(int pn, int cn){
	if(pn==-1)
		return false;
	if(p[pn]->getConceal()==0&&p[pn]->getShow()==0&&p[7]->getCard()[0].getRank()==13)//将K放在空牌堆
		return true;
	if(p[pn]->getConceal()+p[pn]->getShow()!=cn+1)//释放处必须是最后一张牌
		return false;
	if(p[pn]->getCard()[cn].getRank()-1!=p[7]->getCard()[0].getRank())//大小
		return false;
	if((p[pn]->getCard()[cn].getSuit()+p[7]->getCard()[0].getSuit())%2==0)//必异须花色
		return false;
	return true;
}
bool upReleaseable(int pn){
	if(pn<0)
		return false;
	int n=pr[pn]->getNum()-1;
	if(n==-1){
		if(p[7]->getCard()[0].getRank()==1)
			return true;
		else
			return false;
	}	
	if(pr[pn]->getCard()[n].getRank()+1!=p[7]->getCard()[0].getRank())
		return false;
	if(pr[pn]->getCard()[n].getSuit()!=p[7]->getCard()[0].getSuit())
		return false;
	return true;
}
void drawPile(int pn)
{
	if(downFlipFilm&&pn==pilenum){
		int i=0;
		for(;i<p[pn]->getConceal();i++){
		glCallList(53);
		glTranslatef(0.0f,-0.2f,0.01f);
	}
	}
	else
	{

	int i=0;
	//glRotatef(180,0.0f,1.0f,0.0f);
	for(;i<p[pn]->getConceal();i++){
		//glCallList((p[pn]->getCard(i)).getSuit()*13+(p[pn]->getCard(i)).getRank());
		//glTranslatef(0.0f,-0.5f,-0.02f);
		glCallList(53);
		glTranslatef(0.0f,-0.2f,0.01f);
	}
	//glRotatef(180,0.0f,1.0f,0.0f);
	for(;i<p[pn]->getConceal()+p[pn]->getShow();i++){
		glCallList((p[pn]->getCard())[i].getSuit()*13+(p[pn]->getCard())[i].getRank());
		glTranslatef(0.0f,-0.5f,0.01f);
	}


	}	
}

void drawPileLeft(int pln)
{
	if(pln==0){
		for(int i=0;i<pl[pln]->getNum();i++){
			glCallList(53);
			glTranslatef(0.0f,0.0f,0.01f);
		}
	} else {
		if(!flipfilm){
			for(int i=0;i<pl[pln]->getNum();i++){
			glCallList((pl[pln]->getCard())[i].getSuit()*13+(pl[pln]->getCard())[i].getRank());
			glTranslatef(0.0f,0.0f,0.01f);
			}
		} else {
			for(int i=0;i<pl[pln]->getNum()-1;i++){
			glCallList((pl[pln]->getCard())[i].getSuit()*13+(pl[pln]->getCard())[i].getRank());
			glTranslatef(0.0f,0.0f,0.01f);
			}
		}
	}
}
void drawPileRight(int prn){
	for(int i=0;i<pr[prn]->getNum();i++){
		glCallList((pr[prn]->getCard())[i].getSuit()*13+(pr[prn]->getCard())[i].getRank());
		glTranslatef(0.0f,0.0f,0.01f);
	}
}
int DrawGLScene(GLvoid)								// 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// 清除屏幕和深度缓存
	//glLoadIdentity();							// 重置当前的模型观察矩阵
	//glTranslatef(-10.0f,0.0f,-25.0f);						// 物体移入屏幕2个单位
	//glRotatef(xrot,1.0f,0.0f,0.0f);						// 正方体绕X轴旋转
	//glRotatef(45,0.0f,1.0f,0.0f);						// 正方体绕Y轴旋转
	//glRotatef(zrot,0.0f,0.0f,1.0f);						// 正方体绕Z轴旋转
	glEnable(GL_BLEND);									// 启用混合
	for(int i=0;i<7;i++){
		glLoadIdentity();
		glTranslatef(-9.0f+3*i,1.0f,-15.0f);
		drawPile(i);
	}
	for(int i=0;i<2;i++){
		glLoadIdentity();
		glTranslatef(-9.0f+3*i,4.0f,-15.0f);
		drawPileLeft(i);
	}
	for(int i=0;i<4;i++){
		glLoadIdentity();
		glTranslatef(3*i,4.0f,-15.0f);
		glCallList(54);
		drawPileRight(i);
	}
	if(flipfilm){
		glLoadIdentity();
		int i=pl[0]->getNum();
		int n=pl[1]->getNum()-1;
		glTranslatef(-9.0f,4.0f,-15.0f+0.01*i);
		if(flip<=115){
			glTranslatef(flip*3.0f/180,0.0f,-0.01*(i-n)*flip/180+sin(flip*3.1415926/180)*2.373626);
			glRotatef(flip,0.0f,1.0f,0.0f);
			glCallList(53);
			flip+=2;
		}
		else if(flip>115&&flip<=180){
			glTranslatef(flip*3.0f/180,0.0f,-0.01*(i-n)*flip/180+sin(flip*3.1415926/180)*2.373626);
			glRotatef(flip-180,0.0f,1.0f,0.0f);
			glCallList((pl[1]->getCard())[n].getSuit()*13+(pl[1]->getCard())[n].getRank());
			flip+=2;
		}else {
			flip=0;
			flipfilm=false;
			mouseRelease=false;
		}

	}
	if(pileDragFilm&&mouseDown){
		glLoadIdentity();
		glTranslatef(-9.0f+pilenum*3.0f+(mouse_x-down_x)*3.0f/182,1.0f-(p[pilenum]->getConceal())*0.2f-(p[pilenum]->getShow())*0.5f-(mouse_y-down_y)*3.0f/182,-15.0f+0.2f);
		drawPile(7);
	}
	if(plDragFilm&&mouseDown){
		glLoadIdentity();
		float h;
		if(pl[1]->getNum()>=20)
			h=(pl[1]->getNum())*0.01f;
		else
			h=0.2f;
		glTranslatef(-6.0f+(mouse_x-down_x)*3.0f/182,4.0f-(mouse_y-down_y)*3.0f/182,-15.0f+h);
		drawPile(7);
	}
	if(prDragFilm&&mouseDown){
		glLoadIdentity();
		glTranslatef(-9.0f+pilenum*3.0f+(mouse_x-down_x)*3.0f/182,4.0f-(mouse_y-down_y)*3.0f/182,-15.0f+0.2f);
		drawPile(7);
	}
	if(downFlipFilm){
		int c=p[pilenum]->getConceal();
		glLoadIdentity();
		glTranslatef(-9.0f+3*pilenum,1.0f-c*0.2f,-15.0f);
		if(flip<=90){
			glTranslatef(0.0f,0.0f,sin(flip*3.1415926/180)*2.373626);
			glRotatef(flip,0.0f,1.0f,0.0f);
			glCallList(53);
			flip+=2;
		}
		else if(flip>90&&flip<=180){
			glTranslatef(0.0f,0.0f,sin(flip*3.1415926/180)*2.373626);
			glRotatef(flip-180,0.0f,1.0f,0.0f);
			glCallList((p[pilenum]->getCard())[c].getSuit()*13+(p[pilenum]->getCard())[c].getRank());
			flip+=2;
		}else {
			flip=0;
			downFlipFilm=false;
		}
	}
	glDisable(GL_BLEND);								// 禁用混合
	//xrot+=0.03f;								// X 轴旋转
	//yrot+=0.02f;								// Y 轴旋转
	//zrot+=0.04f;								// Z 轴旋转
	return TRUE;								//  一切 OK
}
GLvoid KillGLWindow(GLvoid)							// 正常销毁窗口
{
	if (fullscreen)								// 我们处于全屏模式吗?
	{
	ChangeDisplaySettings(NULL,0);					// 是的话，切换回桌面
	ShowCursor(TRUE);						// 显示鼠标指针
	}
	if (hRC)								// 我们拥有OpenGL渲染描述表吗?
	{
		if (!wglMakeCurrent(NULL,NULL))					// 我们能否释放DC和RC描述表?
		{
		MessageBox(NULL,"释放DC或RC失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC))					// 我们能否删除RC?
		{
			MessageBox(NULL,"释放RC失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;							// 将RC设为 NULL
	}
	if (hDC && !ReleaseDC(hWnd,hDC))					// 我们能否释放 DC?
	{
		MessageBox(NULL,"释放DC失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							// 将 DC 设为 NULL
	}
	if (hWnd && !DestroyWindow(hWnd))					// 能否销毁窗口?
	{
		MessageBox(NULL,"释放窗口句柄失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;							// 将 hWnd 设为 NULL
	}
	if (!UnregisterClass("OpenG",hInstance))				// 能否注销类?
	{
		MessageBox(NULL,"不能注销窗口类。","关闭错误",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;							// 将 hInstance 设为 NULL
	}
}
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;						// 保存查找匹配的结果
	WNDCLASS	wc;							// 窗口类结构
	DWORD		dwExStyle;						// 扩展窗口风格
	DWORD		dwStyle;						// 窗口风格
	RECT WindowRect;							// 取得矩形的左上角和右下角的坐标值
	WindowRect.left=(long)0;						// 将Left   设为 0
	WindowRect.right=(long)width;						// 将Right  设为要求的宽度
	WindowRect.top=(long)0;							// 将Top    设为 0
	WindowRect.bottom=(long)height;						// 将Bottom 设为要求的高度
	fullscreen=fullscreenflag;						// 设置全局全屏标志
	hInstance		= GetModuleHandle(NULL);			// 取得我们窗口的实例
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// 移动时重画，并为窗口取得DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;				// WndProc处理消息
	wc.cbClsExtra		= 0;						// 无额外窗口数据
	wc.cbWndExtra		= 0;						// 无额外窗口数据
	wc.hInstance		= hInstance;					// 设置实例
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);			// 装入缺省图标
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);			// 装入鼠标指针
	wc.hbrBackground	= NULL;						// GL不需要背景
	wc.lpszMenuName		= NULL;						// 不需要菜单
	wc.lpszClassName	= "OpenG";					// 设定类名字
	if (!RegisterClass(&wc))						// 尝试注册窗口类
	{
		MessageBox(NULL,"注册窗口失败","错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 退出并返回FALSE
	}
	if (fullscreen)								// 要尝试全屏模式吗?
	{
		DEVMODE dmScreenSettings;						// 设备模式
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// 确保内存清空为零
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// Devmode 结构的大小
		dmScreenSettings.dmPelsWidth	= width;				// 所选屏幕宽度
		dmScreenSettings.dmPelsHeight	= height;				// 所选屏幕高度
		dmScreenSettings.dmBitsPerPel	= bits;					// 每象素所选的色彩深度
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		// 尝试设置显示模式并返回结果。注: CDS_FULLSCREEN 移去了状态条。
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// 若模式失败，提供两个选项：退出或在窗口内运行。
			if (MessageBox(NULL,"全屏模式在当前显卡上设置失败！\n使用窗口模式？","NeHe G",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;				// 选择窗口模式(Fullscreen=FALSE)
			}
			else
			{
				// 弹出一个对话框，告诉用户程序结束
				MessageBox(NULL,"程序将被关闭","错误",MB_OK|MB_ICONSTOP);
				return FALSE;					//  退出并返回 FALSE
			}
		}
	}
	if (fullscreen)								// 仍处于全屏模式吗?
	{
		dwExStyle=WS_EX_APPWINDOW;					// 扩展窗体风格
		dwStyle=WS_POPUP;						// 窗体风格
		ShowCursor(FALSE);						// 隐藏鼠标指针
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// 扩展窗体风格
		dwStyle=WS_OVERLAPPEDWINDOW;					//  窗体风格
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// 调整窗口达到真正要求的大小
	if (!(hWnd=CreateWindowEx(	dwExStyle,				// 扩展窗体风格
					"OpenG",				// 类名字
					title,					// 窗口标题
					WS_CLIPSIBLINGS |			// 必须的窗体风格属性
					WS_CLIPCHILDREN |			// 必须的窗体风格属性
					dwStyle,				// 选择的窗体属性
					0, 0,					// 窗口位置
					WindowRect.right-WindowRect.left,	// 计算调整好的窗口宽度
					WindowRect.bottom-WindowRect.top,	// 计算调整好的窗口高度
					NULL,					// 无父窗口
					NULL,					// 无菜单
					hInstance,				// 实例
					NULL)))					// 不向WM_CREATE传递任何东东
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL,"不能创建一个窗口设备描述表","错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}
	static	PIXELFORMATDESCRIPTOR pfd=					// /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// 上述格式描述符的大小
		1,								// 版本号
		PFD_DRAW_TO_WINDOW |						// 格式支持窗口
		PFD_SUPPORT_OPENGL |						// 格式必须支持OpenGL
		PFD_DOUBLEBUFFER,						// 必须支持双缓冲
		PFD_TYPE_RGBA,							// 申请 RGBA 格式
		bits,								// 选定色彩深度
		0, 0, 0, 0, 0, 0,						// 忽略的色彩位
		0,								// 无Alpha缓存
		0,								// 忽略Shift Bit
		0,								// 无累加缓存
		0, 0, 0, 0,							// 忽略聚集位
		16,								// 16位 Z-缓存 (深度缓存)
		0,								// 无蒙板缓存
		0,								// 无辅助缓存
		PFD_MAIN_PLANE,							// 主绘图层
		0,								// Reserved
		0, 0, 0								// 忽略层遮罩
	};
	if (!(hDC=GetDC(hWnd)))							// 取得设备描述表了么?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL,"不能创建一种相匹配的像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))				// Windows 找到相应的象素格式了吗?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL,"不能设置像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))				// 能够设置象素格式么?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL,"不能设置像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}
	if (!(hRC=wglCreateContext(hDC)))					// 能否取得着色描述表?
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL,"不能创建OpenGL渲染描述表","错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}
	if(!wglMakeCurrent(hDC,hRC))						// 尝试激活着色描述表
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL,"不能激活当前的OpenGL渲然描述表","错误",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}
	ShowWindow(hWnd,SW_SHOW);						// 显示窗口
	SetForegroundWindow(hWnd);						// 略略提高优先级
	SetFocus(hWnd);								// 设置键盘的焦点至此窗口
	ReSizeGLScene(width, height);						// 设置透视 GL 屏幕
	if (!InitGL())								// 初始化新建的GL窗口
	{
		KillGLWindow();							// 重置显示区
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// 返回 FALSE
	}
	return TRUE;								// 成功
}
LRESULT CALLBACK WndProc(	HWND	hWnd,					// 窗口的句柄
				UINT	uMsg,					// 窗口的消息
				WPARAM	wParam,					// 附加的消息内容
				LPARAM	lParam)					// 附加的消息内容
{
	switch (uMsg)								// 检查Windows消息
	{
	case WM_ACTIVATE:						// 监视窗口激活消息
		{
			if (!HIWORD(wParam))					// 检查最小化状态
			{
				active=TRUE;					// 程序处于激活状态
			}
			else
			{
				active=FALSE;					// 程序不再激活
			}
			return 0;						// 返回消息循环
		}
		case WM_SYSCOMMAND:						// 系统中断命令
		{
			switch (wParam)						// 检查系统调用
			{
				case SC_SCREENSAVE:				// 屏保要运行?
				case SC_MONITORPOWER:				// 显示器要进入节电模式?
				return 0;					// 阻止发生
			}
			break;							// 退出
		}
		case WM_CLOSE:							// 收到Close消息?
		{
			PostQuitMessage(0);					// 发出退出消息
			return 0;						// 返回
		}
		case WM_KEYDOWN:						// 有键按下么?
		{
			keys[wParam] = TRUE;					// 如果是，设为TRUE
			return 0;						// 返回
		}
		case WM_KEYUP:							// 有键放开么?
		{
			keys[wParam] = FALSE;					// 如果是，设为FALSE
			return 0;						// 返回
		}
		case WM_SIZE:							// 调整OpenGL窗口大小
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));		// LoWord=Width,HiWord=Height
			return 0;						// 返回
		}
		case WM_LBUTTONDOWN:
			{
	            mouse_x = LOWORD(lParam);          
				mouse_y = HIWORD(lParam);
				mouseDown=true;
				down_x=mouse_x;
				down_y=mouse_y;
			}
		break;
		case WM_LBUTTONUP:
           {
	            mouse_x = LOWORD(lParam);          
				mouse_y = HIWORD(lParam);
				mouseDown=false;
				mouseRelease=true;
				up_x=mouse_x;
				up_y=mouse_y;
			}
        break;
		case WM_MOUSEMOVE:
			{
	            mouse_x = LOWORD(lParam);          
				mouse_y = HIWORD(lParam);
			}
		break;
	}
	// 向 DefWindowProc传递所有未处理的消息。
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
int WINAPI WinMain(	HINSTANCE	hInstance,				// 当前窗口实例
			HINSTANCE	hPrevInstance,				// 前一个窗口实例
			LPSTR		lpCmdLine,				// 命令行参数
			int		nCmdShow)				// 窗口显示状态
{
	MSG	msg;								// Windowsx消息结构
	BOOL	done=FALSE;							// 用来退出循环的Bool 变量
	// 提示用户选择运行模式
	if (MessageBox(NULL,"你想在全屏模式下运行么？", "设置全屏模式",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;						// FALSE为窗口模式
	}
	// 创建OpenGL窗口
	if (!CreateGLWindow("OPENGL的纸牌游戏",1350,750,16,fullscreen))
	{
		return 0;							// 失败退出
	}
	while(!done)								// 保持循环直到 done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))			// 有消息在等待吗?
		{
			if (msg.message==WM_QUIT)				// 收到退出消息?
			{
				done=TRUE;					// 是，则done=TRUE
			}
			else							// 不是，处理窗口消息
			{
				TranslateMessage(&msg);				// 翻译消息
				DispatchMessage(&msg);				// 发送消息
			}
		}
		else								// 如果没有消息
		{
			// 绘制场景。监视ESC键和来自DrawGLScene()的退出消息
			if (active)						// 程序激活的么?
			{
				if (keys[VK_ESCAPE])				// ESC 按下了么?
				{
					done=TRUE;				// ESC 发出退出信号
				}
				else						// 不是退出的时候，刷新屏幕
				{
					DrawGLScene();				// 绘制场景
					SwapBuffers(hDC);			// 交换缓存 (双缓存)
				}
			}
			if(mouseDown&&!mouseDrag&&!flipfilm&&!downFlipFilm&&down_x%182>68){
				if(down_y>50&&down_y<205){
					//是上面的牌堆
					pilenum=down_x/182;
					if(pilenum==0){
						if(pl[0]->getNum()>0){
							pl[1]->putOne(pl[0]->flipOne());
							flipfilm=true;
							down_x=0;				
						}
						else{
							int j=pl[1]->getNum();
							for(int i=0;i<j;i++)
								pl[0]->putOne(pl[1]->flipOne());
							down_x=0;
						}
					}
					else if(pilenum==1){
						//MessageBox(NULL,"是翻开牌堆","错误",MB_OK|MB_ICONEXCLAMATION);
						if(pl[1]->getNum()>0){
							mouseDrag=true;
							plDragFilm=true;
							p[7]->setShow(1);
							Card c=pl[1]->flipOne();
							p[7]->getCard()[0].setRank(c.getRank());
							p[7]->getCard()[0].setSuit(c.getSuit());
							p[7]->getCard()[0].setShow(true);
						}
						else
							down_x=0;
						
					}
					else if(pilenum>=3&&pilenum<=6){
						//MessageBox(NULL,"是右牌堆","错误",MB_OK|MB_ICONEXCLAMATION);
						if(pr[pilenum-3]->getNum()>0){
							mouseDrag=true;
							prDragFilm=true;
							p[7]->setShow(1);
							Card c=pr[pilenum-3]->fetchOne();
							p[7]->getCard()[0].setRank(c.getRank());
							p[7]->getCard()[0].setSuit(c.getSuit());
							p[7]->getCard()[0].setShow(true);
						}
						else
							down_x=0;
					}
				}
				else if(down_y>238){
					transpos(pilenum,cardnum,down_x,down_y);
					if(cardnum>=0){
						mouseDrag=true;
						pileDragFilm=true;
						p[pilenum]->trasmit(cardnum,p[7]);

					}
					else
						down_x=0;
				}
			}
			if(mouseRelease&&mouseDrag){
				mouseDrag=false;
				mouseRelease=false;
				if(pileDragFilm){
					pileDragFilm=false;	
					int temp=pilenum;
					transpos(pilenum,cardnum,up_x,up_y);
					if(up_y>238){
						if(releaseable(pilenum,cardnum)){
							p[7]->trasmit(0,p[pilenum]);
							if(p[temp]->getConceal()>0&&p[temp]->getShow()==0){
								p[temp]->setShow(1);
								p[temp]->setConceal(p[temp]->getConceal()-1);
								p[temp]->getCard()[p[temp]->getConceal()-1].setShow(true);
								pilenum=temp;
								downFlipFilm=true;
							}
						}
						else{
							p[7]->trasmit(0,p[temp]);
						}
					}	
					else{
						if(upReleaseable(pilenum-3)){
							pr[pilenum-3]->putOne(p[7]->getCard()[0]);
							p[7]->setShow(0);
							if(p[temp]->getConceal()>0&&p[temp]->getShow()==0){
								p[temp]->setShow(1);
								p[temp]->setConceal(p[temp]->getConceal()-1);
								p[temp]->getCard()[p[temp]->getConceal()-1].setShow(true);
								pilenum=temp;
								downFlipFilm=true;
							}
						}
						else{
							p[7]->trasmit(0,p[temp]);
						}
					}
				}
				if(plDragFilm){
					plDragFilm=false;
					transpos(pilenum,cardnum,up_x,up_y);
					if(up_y>238){
						if(releaseable(pilenum,cardnum)){
							p[7]->trasmit(0,p[pilenum]);
						}
						else{
							p[7]->getCard()[0].setShow(false);
							pl[1]->putOne(p[7]->getCard()[0]);
							p[7]->setShow(0);					
						}
					}
					else{
						if(upReleaseable(pilenum-3)){
							pr[pilenum-3]->putOne(p[7]->getCard()[0]);
							p[7]->setShow(0);
						}
						else{
							p[7]->getCard()[0].setShow(false);
							pl[1]->putOne(p[7]->getCard()[0]);
							p[7]->setShow(0);
						}
					}

				}
				if(prDragFilm){
					prDragFilm=false;
					int temp=pilenum;
					transpos(pilenum,cardnum,up_x,up_y);
					if(up_y>238){
						if(releaseable(pilenum,cardnum)){
							p[7]->trasmit(0,p[pilenum]);
						}
						else{
							pr[temp-3]->putOne(p[7]->getCard()[0]);
							p[7]->setShow(0);					
						}
					}
					else{
						if(upReleaseable(pilenum-3)){
							pr[pilenum-3]->putOne(p[7]->getCard()[0]);
							p[7]->setShow(0);
						}
						else{
							pr[temp-3]->putOne(p[7]->getCard()[0]);
							p[7]->setShow(0);	
						}
					}

				}
			
			}
			if (keys[VK_F1])					// F1键按下了么?
			{
				keys[VK_F1]=FALSE;				// 若是，使对应的Key数组中的值为 FALSE
				KillGLWindow();					// 销毁当前的窗口
				fullscreen=!fullscreen;				// 切换 全屏 / 窗口 模式
				// 重建 OpenGL 窗口
				if (!CreateGLWindow("OPENGL的纸牌游戏",1350,750,16,fullscreen))
				{
					return 0;				// 如果窗口未能创建，程序退出
				}
			}
		}
	}
	// 关闭程序
	KillGLWindow();								// 销毁窗口
	glDeleteLists(card,54);						// 从内存中删除256个显示列表
	for(int i=0;i<8;i++)
		delete p[i];
	for(int i=0;i<2;i++)
		delete pl[i];
	for(int i=0;i<4;i++)
		delete pr[i];
	return (msg.wParam);							// 退出程序
}































































