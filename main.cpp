#include <QtGui/QApplication>
#include "wwwdump.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	wwwdump w;
	w.show();
	return a.exec();
}
