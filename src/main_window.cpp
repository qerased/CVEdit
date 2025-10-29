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

    preview_label_ = new QLabel (central);
    preview_label_->setAlignment (Qt::AlignCenter);

    preview_label_->setText("File->ShowTest");

    layout->addWidget (preview_label_);
    layout->setContentsMargins (12, 12, 12, 12);
    central->setLayout (layout);

    setCentralWidget (central);
}

void main_window::create_menus ()
{
    auto * file_menu = menuBar ()->addMenu ("File");
    auto * act_show_test = new QAction ("Show Test");
    connect (act_show_test, &QAction::triggered, this, &main_window::show_test_image);
    file_menu->addAction (act_show_test);

    auto * source_menu = menuBar ()->addMenu ("Source");
    auto * help_menu = menuBar ()->addMenu ("Help");

    Q_UNUSED (source_menu);
    Q_UNUSED (help_menu);
}

void main_window::create_status_bar ()
{
    statusBar ()->showMessage ("Finished");
}

void main_window::update_preview ()
{
    if (current_pixmap_.isNull ())
        return;

    const QSize size = preview_label_->size ();
    if (!size.isValid ())
        return;

    QPixmap scaled = current_pixmap_.scaled (size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    preview_label_->setPixmap (scaled);
    preview_label_->setText (QString ());
}

void main_window::show_test_image ()
{
    QPixmap pixmap (":/test.png");

    if (pixmap.isNull()) {
        preview_label_->setText("Failed to load :/test.jpg");
        statusBar()->showMessage("Failed to load test image", 3000);
        return;
    }

    current_pixmap_ = pixmap;
    update_preview ();
    statusBar()->showMessage("Test image is shown", 3000);
}
