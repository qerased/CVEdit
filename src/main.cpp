#include <QApplication>
#include "main_window.h"

int main (int argc, char *argv[])
{
    QApplication app (argc, argv);
    QApplication::setApplicationName ("CVEdit");

    qRegisterMetaType<sort_mode> ("SortMode");

    main_window window;
    window.show ();

    return app.exec ();
}
