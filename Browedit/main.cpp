#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <BroLib/Log.h>
#include <BroLib/fs.h>
#include <json/json.h>
#include "BrowEdit.h"

Log logger;
BrowEdit* browEdit;


#ifdef WIN32
#include <windows.h>
HWND GetConsoleHwnd();
#endif


void display()
{
	browEdit->draw();
	glutSwapBuffers();
}

void update()
{
	browEdit->update();
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	browEdit->reshape(w,h);
}

void keyboard(unsigned char key, int x, int y)
{
	if(key == 27)
		glutLeaveMainLoop();
	browEdit->keyboard(key);
}



int main(int argc, char* argv[])
{
	browEdit = BrowEdit::getInstance();
	File::registerFileLoader(new PhysicalFileLoader());

	browEdit->config = File::getJson("data/configs/config.borf.json");
#ifdef WIN32
	if(browEdit->config["moveconsole"].asBool() && GetSystemMetrics(80) > 1)
		SetWindowPos(GetConsoleHwnd(), GetConsoleHwnd(), GetSystemMetrics(SM_CXSCREEN),0,0,0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
#endif

	glutInit(&argc, argv);
	glutInitWindowSize(1900, 1000);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Browedit 2.0");
	if(int errorCode = glewInit() != GLEW_OK)
		logger<<"Error initializing glew: "<<(char*)glewGetErrorString(errorCode)<<Log::newline;
	else
		logger<<"Initialized Glew "<<(char*)glewGetString(GLEW_VERSION)<<", OpenGL Version "<<(char*)glGetString(GL_VERSION)<<Log::newline;


	glutIdleFunc(update);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	browEdit->init();
	glutMainLoop();


	delete browEdit;
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