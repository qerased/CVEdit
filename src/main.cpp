#include <QApplication>
#include "main_window.h"

int main (int argc, char *argv[])
{
    QApplication app (argc, argv);
    QApplication::setApplicationName ("CVEdit");

    qRegisterMetaType<psort::sort_mode>  ("SortMode");
    qRegisterMetaType<psort::sort_scope> ("SortScope");
    qRegisterMetaType<psort::sort_axis>  ("SortAxis");

    main_window window;
    window.show ();

    return app.exec ();
}
