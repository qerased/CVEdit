#include "main_window.h"
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

main_window::main_window (QWidget *parent)
{
    setWindowTitle ("CVEdit");
    resize (1280, 720);

    create_ui ();
    create_menus ();
    create_status_bar ();
}

void main_window::create_ui ()
{
    auto * central = new QWidget (this);
    auto * layout = new QVBoxLayout (central);

    auto * label = new QLabel ("Welcome to CVEdit", central);
    label->setAlignment (Qt::AlignCenter);

    layout->addWidget (label);
    layout->setContentsMargins (12, 12, 12, 12);
    central->setLayout (layout);

    setCentralWidget (central);
}

void main_window::create_menus ()
{
    auto * file_menu = menuBar ()->addMenu ("File");
    auto * source_menu = menuBar ()->addMenu ("Source");
    auto * help_menu = menuBar ()->addMenu ("Help");

    Q_UNUSED (file_menu);
    Q_UNUSED (source_menu);
    Q_UNUSED (help_menu);
}

void main_window::create_status_bar ()
{
    statusBar ()->showMessage ("Finished");
}
