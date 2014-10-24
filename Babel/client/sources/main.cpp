#include <qapplication.h>
#include "Babel.hpp"
#include <QMetaType>
#include "Sound.hpp"

int main(int ac, char **av) {
	QApplication	app(ac, av);
	Babel			babel;

	qRegisterMetaType<Sound::Encoded>();
	babel.run();
	return app.exec();
}