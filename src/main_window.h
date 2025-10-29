#ifndef CVEDIT_MAIN_WINDOW_H
#define CVEDIT_MAIN_WINDOW_H
#include <QMainWindow>

class main_window : public QMainWindow
{
    Q_OBJECT
public:
    explicit main_window (QWidget *parent = nullptr);

private:
    void create_ui ();
    void create_menus ();
    void create_status_bar ();
};


#endif //CVEDIT_MAIN_WINDOW_H