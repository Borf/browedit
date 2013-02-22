#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <Log.h>

Log log;


#ifdef WIN32
#include <Windows.h>
HWND GetConsoleHwnd();
#endif


void display()
{
	glutSwapBuffers();
}

void reshape(int w, int h)
{

}

void keyboard(unsigned char key, int x, int y)
{
	if(key == 27)
		exit(0);
}



int main(int argc, char* argv[])
{
#ifdef WIN32
	if(GetSystemMetrics(80) > 1)
		SetWindowPos(GetConsoleHwnd(), GetConsoleHwnd(), GetSystemMetrics(SM_CXSCREEN),0,0,0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
#endif

	glutInit(&argc, argv);
	glutInitWindowSize(1920, 1080);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Browedit 2.0");
	if(int errorCode = glewInit() != GLEW_OK)
		log<<"Error initializing glew: "<<(char*)glewGetErrorString(errorCode)<<Log::newline;
	else
		log<<"Initialized Glew "<<(char*)glewGetString(GLEW_VERSION)<<", OpenGL Version "<<(char*)glGetString(GL_VERSION)<<Log::newline;


	

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}









#ifdef WIN32
HWND GetConsoleHwnd()
{
	HWND hwndFound;
	char TempWindowTitle[1024];
	char WindowTitle[1024];
	GetConsoleTitleA(WindowTitle, 1024);
	sprintf_s(TempWindowTitle,1024, "%d/%d", GetTickCount(), GetCurrentProcessId());
	SetConsoleTitleA(TempWindowTitle);
	Sleep(40);
	hwndFound=FindWindowA(NULL, TempWindowTitle);
	SetConsoleTitleA(WindowTitle);
	return(hwndFound);
}
#endif