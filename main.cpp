#include "robot_connect.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Robot_Connect w;
	w.show();
	return a.exec();
}
