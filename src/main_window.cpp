#include "main_window.h"
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QTimer>
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

    preview_label_->setText("Open Image or Start Webcam");

    preview_label_->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Ignored);
    preview_label_->setMinimumSize (100, 100);

    layout->addWidget (preview_label_);
    layout->setContentsMargins (12, 12, 12, 12);
    central->setLayout (layout);

    setCentralWidget (central);


    grab_timer_ = new QTimer (this);
    grab_timer_->setInterval (33);
    connect (grab_timer_, &QTimer::timeout, this, &main_window::on_grab_tick);
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
    act_start_webcam = new QAction ("Start Webcam", this);
    connect (act_start_webcam, &QAction::triggered, this, &main_window::start_webcam);
    source_menu->addAction (act_start_webcam);

    act_stop_ = new QAction ("Stop Source", this);
    act_stop_->setEnabled (false);
    connect (act_stop_, &QAction::triggered, this, &main_window::stop_source);
    source_menu->addAction (act_stop_);

    auto * help_menu = menuBar ()->addMenu ("Help");

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
    const QString filter = "Images (*.png *.jpg *.jpeg *.bmp *.tiff);;All (*.*)";
    const QString path = QFileDialog::getOpenFileName (this, "Open Image", QString (), filter);

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

void main_window::start_webcam ()
{
    stop_source ();

    source_ = std::make_unique<video_source> ();

    if (!source_->open_webcam (0))
    {
        source_.reset ();
        preview_label_->setText ("Failed to open Webcam");
        statusBar ()->showMessage ("Failed to open Webcam", 3000);
        return;
    }

    double fps = source_->fps ();
    if (fps > 1. && fps < 240.)
    {
        int ms = static_cast<int> (1000 / fps);
        grab_timer_->setInterval (std::max (10, ms));
    }
    else
    {
        grab_timer_->setInterval (33);
    }

    act_start_webcam->setEnabled (false);
    act_stop_->setEnabled (true);
    grab_timer_->start ();

    preview_label_->setText ("Starting Webcam");
    statusBar()->showMessage ("Started Webcam", 3000);
}

void main_window::stop_source()
{
    if (grab_timer_->isActive ())
        grab_timer_->stop ();

    if (source_)
    {
        source_->close ();
        source_.reset ();
    }

    act_start_webcam->setEnabled (true);
    act_stop_->setEnabled (false);
}

void main_window::on_grab_tick ()
{
    if (!source_ || !source_->is_opened ())
    {
        stop_source ();
        return;
    }

    cv::Mat frame;
    if (!source_->read (frame) || frame.empty ())
    {
        stop_source ();
        preview_label_->setText ("Stopped source");
        statusBar ()->showMessage ("Failed to get frames from source", 3000);
        return;
    }

    cv::flip (frame, frame, 1);

    current_pixmap_ = utils::mat_to_pixmap (frame);
    update_preview ();
}

