#include <windows.h>		// Windows��ͷ�ļ�
#include "glew.h"			// �������µ�gl.h,glu.h��
#include <stdio.h>			// ��׼����/������ͷ�ļ�
#include <math.h>			// ��ѧ�⺯��
#include "glaux.h"			// GLaux���ͷ�ļ�
#include "card.h"
#include <math.h>


HGLRC           hRC=NULL;							// ������ɫ��������
HDC             hDC=NULL;							// OpenGL��Ⱦ��������
HWND            hWnd=NULL;							// �������ǵĴ��ھ��
HINSTANCE       hInstance;							// ��������ʵ��
GLfloat     cl=0.140625f;
GLfloat		cr=0.859375f;
bool	keys[256];								// ������̰���������
bool	active=TRUE;								// ���ڵĻ��־��ȱʡΪTRUE
bool	fullscreen=TRUE;							// ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ
GLuint	texture[55];					// ����5�������־
GLuint	loop;						// ѭ������
GLfloat		xrot;			// X ��ת��
GLfloat		yrot;			// Y ��ת��
GLfloat		zrot;			// Z ��ת��
GLuint		card;
Pile *p[8];					//p[7]����ʱ�ƶѣ����ж���ʱ������
PileRight *pr[4];					
PileLeft *pl[2];
int	mouse_x, mouse_y;      //�������
int down_x;                //�������ʱ�������
int down_y;
int up_x;                  //�ɿ�������ʱ�������
int up_y;
int pilenum;				//���������ת��Ϊ�ƶѺźͿ���
int cardnum;
bool mouseDown=false;		//��갴�£�
bool mouseRelease=false;	//��갴�º����ɿ���
bool mouseDrag=false;		//������ж��ƣ�
bool pileDragFilm=false;	//�Ƿ������϶��·��ƶѣ�
bool flipfilm=false;//�Ƿ����ڲ��ŷ��ƶ���
bool downFlipFilm=false;//���ƶѵķ��ƶ���
bool plDragFilm=false;	//���ƶ��϶�����
bool prDragFilm=false;	//���ƶ��϶�����
GLfloat    flip;		//���ƶ�������ת�Ƕ�

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);				// WndProc�Ķ���

GLvoid BuildLists()								// �������ӵ���ʾ�б�
{

	card=glGenLists(54);							// ����������ʾ�б������
	for(int i=0;i<53;i++){
	glNewList(card+i,GL_COMPILE);					// ������һ����ʾ�б�

		//glEnable(GL_BLEND);									// ���û��
	glBlendFunc(GL_DST_COLOR,GL_ZERO);				// ʹ�úڰס���ģ�������Ļ��ɫ
	glBindTexture(GL_TEXTURE_2D, texture[0]);		// ѡ��ڶ�������
		glBegin(GL_QUADS);								// �����ı���
			glTexCoord2f(cl, 0.0f); glVertex3f(-0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 0.0f); glVertex3f( 0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 1.0f); glVertex3f( 0.9f,  1.25f,  0.0f);	
			glTexCoord2f(cl, 1.0f); glVertex3f(-0.9f,  1.25f,  0.0f);	
		glEnd();
	glBlendFunc(GL_ONE, GL_ONE);					// ������2���Ƶ���Ļ

	glBindTexture(GL_TEXTURE_2D, texture[i+1]);		// ѡ��ڶ�������
		glBegin(GL_QUADS);								// �����ı���
			glTexCoord2f(cl, 0.0f); glVertex3f(-0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 0.0f); glVertex3f( 0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 1.0f); glVertex3f( 0.9f,  1.25f,  0.0f);	
			glTexCoord2f(cl, 1.0f); glVertex3f(-0.9f,  1.25f,  0.0f);	
		glEnd();
	//glDisable(GL_BLEND);								// ���û��

	//glBindTexture(GL_TEXTURE_2D, texture[53]);		// ѡ��ڶ�������
	//glBegin(GL_QUADS);
	//	// ����
	//	glTexCoord2f(0.8672f, 0.0f); glVertex3f(-0.9f, -1.25f, -0.01f);	// ������ı��ε�����
	//	glTexCoord2f(0.8762f, 1.0f); glVertex3f(-0.9f,  1.25f, -0.01f);	// ������ı��ε�����
	//	glTexCoord2f(0.1328f, 1.0f); glVertex3f( 0.9f,  1.25f, -0.01f);	// ������ı��ε�����
	//	glTexCoord2f(0.1328f, 0.0f); glVertex3f( 0.9f, -1.25f, -0.01f);	// ������ı��ε�����
	//glEnd();

	glEndList();								// ��һ����ʾ�б����
	}
	glNewList(card+53,GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, texture[54]);		// ѡ��ڶ�������
		glBegin(GL_QUADS);								// �����ı���
			glTexCoord2f(cl, 0.0f); glVertex3f(-0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 0.0f); glVertex3f( 0.9f, -1.25f,  0.0f);	
			glTexCoord2f(cr, 1.0f); glVertex3f( 0.9f,  1.25f,  0.0f);	
			glTexCoord2f(cl, 1.0f); glVertex3f(-0.9f,  1.25f,  0.0f);	
		glEnd();
	glEndList();
}


AUX_RGBImageRec *LoadBMP(CHAR *Filename)				// ����λͼͼ��
{
	FILE *File=NULL;									// �ļ����
	if (!Filename)										// ȷ���ļ������ṩ
	{
		return NULL;									// ���û�ṩ������ NULL
	}
	File=fopen(Filename,"r");							// ���Դ��ļ�
	if (File)											// �ļ�����ô?
	{
		fclose(File);									// �رվ��
		return auxDIBImageLoadA(Filename);				// ����λͼ������ָ��
	}
	return NULL;										// �������ʧ�ܣ����� NULL
}
int LoadGLTextures()						
{

	int Status=TRUE;						
	AUX_RGBImageRec *TextureImage[55];					// ��������5����������ݽṹ
	memset(TextureImage,0,sizeof(void *)*55);			// ��ʼ��
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
		glGenTextures(55, &texture[0]);					// ����5������

		for (loop=0; loop<55; loop++)					// ѭ������5������
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
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)				// ����OpenGL���ڴ�С
{
	if (height==0)								// ��ֹ�����
	{
		height=1;							// ��Height��Ϊ1
	}
	glViewport(0, 0, width, height);					// ���õ�ǰ���ӿ�
	glMatrixMode(GL_PROJECTION);						// ѡ��ͶӰ����
	glLoadIdentity();							// ����ͶӰ����
	// �����ӿڵĴ�С
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);						// ѡ��ģ�͹۲����
	glLoadIdentity();							// ����ģ�͹۲����
}
void initCard(){
	bool whole[52];//���ڳ�ʼ��ʱʹ�á���52��bool����52�����Ƿ��ù���˳��Ϊ����1~13������1~13��÷��1~13����Ƭ1~13��
	for(int i=0;i<52;i++)
		whole[i]=false;
	int used=0;//whole������ʹ�ù����Ƶ�������ͬ�����ڳ�ʼ��ʹ�ã�Ϊ���������
	srand( (unsigned)time( NULL ) );//srand()��������һ���Ե�ǰʱ�俪ʼ���������.Ӧ�÷���for��ѭ�����ǰ�� ��ȻҪ�ܳ�ʱ��ȴ�
	for(int i=0;i<7;i++)
		p[i]=new Pile(1,i,whole,used);//��ʼ��7�����ƶѡ�
	pl[0]=new PileLeft(24,whole,used);//��ʼ������ƶѡ�
	pl[1]=new PileLeft();
	p[7]=new Pile();
	for(int i=0;i<4;i++)
		pr[i]=new PileRight();
}

int InitGL(GLvoid)										// �˴���ʼ��OpenGL������������
{
	if (!LoadGLTextures())								// ������������������
	{
		return FALSE;									// ���δ�����룬����FALSE
	}

	glEnable(GL_TEXTURE_2D);							// ��������ӳ��
	glShadeModel(GL_SMOOTH);							// ������Ӱƽ��
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// ��ɫ����
	glClearDepth(1.0f);									// ������Ȼ���
	glEnable(GL_DEPTH_TEST);							// ������Ȳ���
	glDepthFunc(GL_LEQUAL);								// ������Ȳ��Ե�����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// ����ϵͳ��͸�ӽ�������
	BuildLists();										// ������ʾ�б�
	initCard();											//���ƣ���ʼ���ƶ�
	return TRUE;										// ��ʼ�� OK
}
void transpos(int &pn,int &cn,int x,int y){
	//���������ת��Ϊ�ƶѺźͿ���,ע�⵱�ƶ�Ϊ��ʱcnҲ�����-1
	if(x%182>68)
		pn=x/182;
	else
		pn=-1;
	int c=p[pn]->getConceal();
	int s=p[pn]->getShow();
	if((y>238+c*12+(s-1)*30+150)||(y<238+c*12))//�Ǳ����ƻ��߳���ȥ��
		cn=-1;
	else if(y<238+c*12+(s-1)*30)
		cn=(y-238-12*c)/30+c;
	else
		cn=c+s-1;
}
bool releaseable(int pn, int cn){
	if(pn==-1)
		return false;
	if(p[pn]->getConceal()==0&&p[pn]->getShow()==0&&p[7]->getCard()[0].getRank()==13)//��K���ڿ��ƶ�
		return true;
	if(p[pn]->getConceal()+p[pn]->getShow()!=cn+1)//�ͷŴ����������һ����
		return false;
	if(p[pn]->getCard()[cn].getRank()-1!=p[7]->getCard()[0].getRank())//��С
		return false;
	if((p[pn]->getCard()[cn].getSuit()+p[7]->getCard()[0].getSuit())%2==0)//�����뻨ɫ
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
int DrawGLScene(GLvoid)								// �����￪ʼ�������еĻ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// �����Ļ����Ȼ���
	//glLoadIdentity();							// ���õ�ǰ��ģ�͹۲����
	//glTranslatef(-10.0f,0.0f,-25.0f);						// ����������Ļ2����λ
	//glRotatef(xrot,1.0f,0.0f,0.0f);						// ��������X����ת
	//glRotatef(45,0.0f,1.0f,0.0f);						// ��������Y����ת
	//glRotatef(zrot,0.0f,0.0f,1.0f);						// ��������Z����ת
	glEnable(GL_BLEND);									// ���û��
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
	glDisable(GL_BLEND);								// ���û��
	//xrot+=0.03f;								// X ����ת
	//yrot+=0.02f;								// Y ����ת
	//zrot+=0.04f;								// Z ����ת
	return TRUE;								//  һ�� OK
}
GLvoid KillGLWindow(GLvoid)							// �������ٴ���
{
	if (fullscreen)								// ���Ǵ���ȫ��ģʽ��?
	{
	ChangeDisplaySettings(NULL,0);					// �ǵĻ����л�������
	ShowCursor(TRUE);						// ��ʾ���ָ��
	}
	if (hRC)								// ����ӵ��OpenGL��Ⱦ��������?
	{
		if (!wglMakeCurrent(NULL,NULL))					// �����ܷ��ͷ�DC��RC������?
		{
		MessageBox(NULL,"�ͷ�DC��RCʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC))					// �����ܷ�ɾ��RC?
		{
			MessageBox(NULL,"�ͷ�RCʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;							// ��RC��Ϊ NULL
	}
	if (hDC && !ReleaseDC(hWnd,hDC))					// �����ܷ��ͷ� DC?
	{
		MessageBox(NULL,"�ͷ�DCʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							// �� DC ��Ϊ NULL
	}
	if (hWnd && !DestroyWindow(hWnd))					// �ܷ����ٴ���?
	{
		MessageBox(NULL,"�ͷŴ��ھ��ʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;							// �� hWnd ��Ϊ NULL
	}
	if (!UnregisterClass("OpenG",hInstance))				// �ܷ�ע����?
	{
		MessageBox(NULL,"����ע�������ࡣ","�رմ���",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;							// �� hInstance ��Ϊ NULL
	}
}
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;						// �������ƥ��Ľ��
	WNDCLASS	wc;							// ������ṹ
	DWORD		dwExStyle;						// ��չ���ڷ��
	DWORD		dwStyle;						// ���ڷ��
	RECT WindowRect;							// ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
	WindowRect.left=(long)0;						// ��Left   ��Ϊ 0
	WindowRect.right=(long)width;						// ��Right  ��ΪҪ��Ŀ��
	WindowRect.top=(long)0;							// ��Top    ��Ϊ 0
	WindowRect.bottom=(long)height;						// ��Bottom ��ΪҪ��ĸ߶�
	fullscreen=fullscreenflag;						// ����ȫ��ȫ����־
	hInstance		= GetModuleHandle(NULL);			// ȡ�����Ǵ��ڵ�ʵ��
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;				// WndProc������Ϣ
	wc.cbClsExtra		= 0;						// �޶��ⴰ������
	wc.cbWndExtra		= 0;						// �޶��ⴰ������
	wc.hInstance		= hInstance;					// ����ʵ��
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);			// װ��ȱʡͼ��
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);			// װ�����ָ��
	wc.hbrBackground	= NULL;						// GL����Ҫ����
	wc.lpszMenuName		= NULL;						// ����Ҫ�˵�
	wc.lpszClassName	= "OpenG";					// �趨������
	if (!RegisterClass(&wc))						// ����ע�ᴰ����
	{
		MessageBox(NULL,"ע�ᴰ��ʧ��","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// �˳�������FALSE
	}
	if (fullscreen)								// Ҫ����ȫ��ģʽ��?
	{
		DEVMODE dmScreenSettings;						// �豸ģʽ
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// ȷ���ڴ����Ϊ��
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// Devmode �ṹ�Ĵ�С
		dmScreenSettings.dmPelsWidth	= width;				// ��ѡ��Ļ���
		dmScreenSettings.dmPelsHeight	= height;				// ��ѡ��Ļ�߶�
		dmScreenSettings.dmBitsPerPel	= bits;					// ÿ������ѡ��ɫ�����
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		// ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬����
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// ��ģʽʧ�ܣ��ṩ����ѡ��˳����ڴ��������С�
			if (MessageBox(NULL,"ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ�ܣ�\nʹ�ô���ģʽ��","NeHe G",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;				// ѡ�񴰿�ģʽ(Fullscreen=FALSE)
			}
			else
			{
				// ����һ���Ի��򣬸����û��������
				MessageBox(NULL,"���򽫱��ر�","����",MB_OK|MB_ICONSTOP);
				return FALSE;					//  �˳������� FALSE
			}
		}
	}
	if (fullscreen)								// �Դ���ȫ��ģʽ��?
	{
		dwExStyle=WS_EX_APPWINDOW;					// ��չ������
		dwStyle=WS_POPUP;						// ������
		ShowCursor(FALSE);						// �������ָ��
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// ��չ������
		dwStyle=WS_OVERLAPPEDWINDOW;					//  ������
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// �������ڴﵽ����Ҫ��Ĵ�С
	if (!(hWnd=CreateWindowEx(	dwExStyle,				// ��չ������
					"OpenG",				// ������
					title,					// ���ڱ���
					WS_CLIPSIBLINGS |			// ����Ĵ���������
					WS_CLIPCHILDREN |			// ����Ĵ���������
					dwStyle,				// ѡ��Ĵ�������
					0, 0,					// ����λ��
					WindowRect.right-WindowRect.left,	// ��������õĴ��ڿ��
					WindowRect.bottom-WindowRect.top,	// ��������õĴ��ڸ߶�
					NULL,					// �޸�����
					NULL,					// �޲˵�
					hInstance,				// ʵ��
					NULL)))					// ����WM_CREATE�����κζ���
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"���ܴ���һ�������豸������","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	static	PIXELFORMATDESCRIPTOR pfd=					// /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// ������ʽ�������Ĵ�С
		1,								// �汾��
		PFD_DRAW_TO_WINDOW |						// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |						// ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,						// ����֧��˫����
		PFD_TYPE_RGBA,							// ���� RGBA ��ʽ
		bits,								// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,						// ���Ե�ɫ��λ
		0,								// ��Alpha����
		0,								// ����Shift Bit
		0,								// ���ۼӻ���
		0, 0, 0, 0,							// ���Ծۼ�λ
		16,								// 16λ Z-���� (��Ȼ���)
		0,								// ���ɰ建��
		0,								// �޸�������
		PFD_MAIN_PLANE,							// ����ͼ��
		0,								// Reserved
		0, 0, 0								// ���Բ�����
	};
	if (!(hDC=GetDC(hWnd)))							// ȡ���豸��������ô?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"���ܴ���һ����ƥ������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))				// Windows �ҵ���Ӧ�����ظ�ʽ����?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))				// �ܹ��������ظ�ʽô?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	if (!(hRC=wglCreateContext(hDC)))					// �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"���ܴ���OpenGL��Ⱦ������","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	if(!wglMakeCurrent(hDC,hRC))						// ���Լ�����ɫ������
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"���ܼ��ǰ��OpenGL��Ȼ������","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	ShowWindow(hWnd,SW_SHOW);						// ��ʾ����
	SetForegroundWindow(hWnd);						// ����������ȼ�
	SetFocus(hWnd);								// ���ü��̵Ľ������˴���
	ReSizeGLScene(width, height);						// ����͸�� GL ��Ļ
	if (!InitGL())								// ��ʼ���½���GL����
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	return TRUE;								// �ɹ�
}
LRESULT CALLBACK WndProc(	HWND	hWnd,					// ���ڵľ��
				UINT	uMsg,					// ���ڵ���Ϣ
				WPARAM	wParam,					// ���ӵ���Ϣ����
				LPARAM	lParam)					// ���ӵ���Ϣ����
{
	switch (uMsg)								// ���Windows��Ϣ
	{
	case WM_ACTIVATE:						// ���Ӵ��ڼ�����Ϣ
		{
			if (!HIWORD(wParam))					// �����С��״̬
			{
				active=TRUE;					// �����ڼ���״̬
			}
			else
			{
				active=FALSE;					// �����ټ���
			}
			return 0;						// ������Ϣѭ��
		}
		case WM_SYSCOMMAND:						// ϵͳ�ж�����
		{
			switch (wParam)						// ���ϵͳ����
			{
				case SC_SCREENSAVE:				// ����Ҫ����?
				case SC_MONITORPOWER:				// ��ʾ��Ҫ����ڵ�ģʽ?
				return 0;					// ��ֹ����
			}
			break;							// �˳�
		}
		case WM_CLOSE:							// �յ�Close��Ϣ?
		{
			PostQuitMessage(0);					// �����˳���Ϣ
			return 0;						// ����
		}
		case WM_KEYDOWN:						// �м�����ô?
		{
			keys[wParam] = TRUE;					// ����ǣ���ΪTRUE
			return 0;						// ����
		}
		case WM_KEYUP:							// �м��ſ�ô?
		{
			keys[wParam] = FALSE;					// ����ǣ���ΪFALSE
			return 0;						// ����
		}
		case WM_SIZE:							// ����OpenGL���ڴ�С
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));		// LoWord=Width,HiWord=Height
			return 0;						// ����
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
	// �� DefWindowProc��������δ�������Ϣ��
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
int WINAPI WinMain(	HINSTANCE	hInstance,				// ��ǰ����ʵ��
			HINSTANCE	hPrevInstance,				// ǰһ������ʵ��
			LPSTR		lpCmdLine,				// �����в���
			int		nCmdShow)				// ������ʾ״̬
{
	MSG	msg;								// Windowsx��Ϣ�ṹ
	BOOL	done=FALSE;							// �����˳�ѭ����Bool ����
	// ��ʾ�û�ѡ������ģʽ
	if (MessageBox(NULL,"������ȫ��ģʽ������ô��", "����ȫ��ģʽ",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;						// FALSEΪ����ģʽ
	}
	// ����OpenGL����
	if (!CreateGLWindow("OPENGL��ֽ����Ϸ",1350,750,16,fullscreen))
	{
		return 0;							// ʧ���˳�
	}
	while(!done)								// ����ѭ��ֱ�� done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))			// ����Ϣ�ڵȴ���?
		{
			if (msg.message==WM_QUIT)				// �յ��˳���Ϣ?
			{
				done=TRUE;					// �ǣ���done=TRUE
			}
			else							// ���ǣ���������Ϣ
			{
				TranslateMessage(&msg);				// ������Ϣ
				DispatchMessage(&msg);				// ������Ϣ
			}
		}
		else								// ���û����Ϣ
		{
			// ���Ƴ���������ESC��������DrawGLScene()���˳���Ϣ
			if (active)						// ���򼤻��ô?
			{
				if (keys[VK_ESCAPE])				// ESC ������ô?
				{
					done=TRUE;				// ESC �����˳��ź�
				}
				else						// �����˳���ʱ��ˢ����Ļ
				{
					DrawGLScene();				// ���Ƴ���
					SwapBuffers(hDC);			// �������� (˫����)
				}
			}
			if(mouseDown&&!mouseDrag&&!flipfilm&&!downFlipFilm&&down_x%182>68){
				if(down_y>50&&down_y<205){
					//��������ƶ�
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
						//MessageBox(NULL,"�Ƿ����ƶ�","����",MB_OK|MB_ICONEXCLAMATION);
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
						//MessageBox(NULL,"�����ƶ�","����",MB_OK|MB_ICONEXCLAMATION);
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
			if (keys[VK_F1])					// F1��������ô?
			{
				keys[VK_F1]=FALSE;				// ���ǣ�ʹ��Ӧ��Key�����е�ֵΪ FALSE
				KillGLWindow();					// ���ٵ�ǰ�Ĵ���
				fullscreen=!fullscreen;				// �л� ȫ�� / ���� ģʽ
				// �ؽ� OpenGL ����
				if (!CreateGLWindow("OPENGL��ֽ����Ϸ",1350,750,16,fullscreen))
				{
					return 0;				// �������δ�ܴ����������˳�
				}
			}
		}
	}
	// �رճ���
	KillGLWindow();								// ���ٴ���
	glDeleteLists(card,54);						// ���ڴ���ɾ��256����ʾ�б�
	for(int i=0;i<8;i++)
		delete p[i];
	for(int i=0;i<2;i++)
		delete pl[i];
	for(int i=0;i<4;i++)
		delete pr[i];
	return (msg.wParam);							// �˳�����
}































































