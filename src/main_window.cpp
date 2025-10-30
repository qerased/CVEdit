#include "main_window.h"
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <opencv2/core.hpp>

#include "utils.h"

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

    preview_label_->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Ignored);
    preview_label_->setMinimumSize (100, 100);

    layout->addWidget (preview_label_);
    layout->setContentsMargins (12, 12, 12, 12);
    central->setLayout (layout);

    setCentralWidget (central);
}

void main_window::create_menus ()
{
    auto * file_menu = menuBar ()->addMenu ("File");
    auto * act_show_test = new QAction ("Show Test", this);
    connect (act_show_test, &QAction::triggered, this, &main_window::show_test_image);
    file_menu->addAction (act_show_test);

    auto * act_open_image = new QAction ("Open Image", this);
    connect (act_open_image, &QAction::triggered, this, &main_window::open_image_from_disk);
    file_menu->addAction (act_open_image);


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

void main_window::resizeEvent (QResizeEvent* e)
{
    QMainWindow::resizeEvent(e);
    if (!current_pixmap_.isNull())
        update_preview ();
}

void main_window::open_image_from_disk ()
{
    const QString path = QFileDialog::getOpenFileName (this, "Open Image", QString ());

    if (path.isEmpty ()) return;

    QString err;
    cv::Mat mat = utils::load_image_mat (path, &err);
    if (mat.empty ())
    {
        preview_label_->setText (QString ("Unable to open image:\n%1\n%2").arg (path, err));
        statusBar ()->showMessage ("Failed to load image from disk", 3000);
        return;
    }

    current_pixmap_ = utils::mat_to_pixmap (mat);
    update_preview ();
    statusBar ()->showMessage (QString ("Image opened: %1").arg (QFileInfo (path).fileName ()), 3000);
}