#include "main_window.h"
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QTimer>
#include <QGroupBox>
#include <QScrollArea>
#include <opencv2/core.hpp>

#include "utils.h"

main_window::main_window (QWidget *parent)
{
    setWindowTitle ("CVEdit");
    resize (1280, 720);

    create_ui ();
    create_menus ();
    create_status_bar ();
    create_filters_dock ();

    filter_grayscale_ = filter_chain_.add<filter_grayscale> ();
    filter_blur_ = filter_chain_.add<filter_blur> ();
    filter_shake_ = filter_chain_.add<filter_shake> ();
    filter_sort_ = filter_chain_.add<filter_sort> ();
    filter_canny_ = filter_chain_.add<filter_canny> (); /// TODO: add smart ordering and prevent canny before sort
    filter_kuwahara_ = filter_chain_.add<filter_kuwahara> ();

    filter_grayscale_->set_enabled (false);
    filter_blur_->set_enabled (false);
    filter_canny_->set_enabled (false);
    filter_shake_->set_enabled (false);
    filter_sort_->set_enabled (false);
    filter_kuwahara_->set_enabled (false);
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
    grab_timer_->start ();
}

void main_window::create_filters_dock ()
{
    dock_filters_ = new QDockWidget ("Filters", this);
    dock_filters_->setObjectName ("dock_filters");
    dock_filters_->setAllowedAreas (Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);

    dock_filters_->setFeatures (QDockWidget::DockWidgetMovable);

    setDockOptions (QMainWindow::AllowTabbedDocks |
                    QMainWindow::GroupedDragging |
                    QMainWindow::AnimatedDocks);

    auto * scroll = new QScrollArea (dock_filters_);
    scroll->setWidgetResizable (true);
    scroll->setFrameShape (QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy (Qt::ScrollBarAsNeeded);

    auto * w = new QWidget (scroll);
    auto * v = new QVBoxLayout (w);
    v->setContentsMargins (8, 8, 8, 8);
    v->setSpacing (8);

    /// grayscale
    {
        auto * box = new QGroupBox ("Grayscale");
        auto * h = new QHBoxLayout (box);
        chk_grayscale_ = new QCheckBox ("Enable", box);
        chk_grayscale_->setChecked (false);
        h->addWidget (chk_grayscale_);
        box->setLayout (h);
        v->addWidget (box);
        connect (chk_grayscale_, &QCheckBox::toggled, this, &main_window::on_toggle_grayscale);
    }

    /// blur
    {
        auto * box = new QGroupBox ("Gaussian Blur");
        auto * h = new QHBoxLayout (box);
        chk_blur_ = new QCheckBox ("Enable", box);
        chk_blur_->setChecked (false);
        h->addWidget (chk_blur_);

        slider_blur_ = new QSlider (Qt::Horizontal, box);
        slider_blur_->setValue (5);
        slider_blur_->setMinimum (1);
        slider_blur_->setMaximum (100);
        h->addWidget (slider_blur_);

        box->setLayout (h);
        v->addWidget (box);

        connect (chk_blur_, &QCheckBox::toggled, this, &main_window::on_toggle_blur);
        connect (slider_blur_, &QSlider::valueChanged, this, &main_window::on_slider_blur);
    }

    /// canny
    {
        auto * box = new QGroupBox ("Canny");
        auto * h = new QHBoxLayout (box);
        chk_canny_ = new QCheckBox ("Enable", box);
        chk_canny_->setChecked (false);
        h->addWidget (chk_canny_);

        slider_canny_thr1 = new QSlider (Qt::Horizontal, box);
        slider_canny_thr1->setMinimum (1);
        slider_canny_thr1->setMaximum (200);
        slider_canny_thr1->setValue (100);
        h->addWidget (slider_canny_thr1);

        slider_canny_thr2 = new QSlider (Qt::Horizontal, box);
        slider_canny_thr2->setMinimum (1);
        slider_canny_thr2->setMaximum (200);
        slider_canny_thr2->setValue (200);
        h->addWidget (slider_canny_thr2);

        box->setLayout (h);
        v->addWidget (box);

        connect (chk_canny_, &QCheckBox::toggled, this, &main_window::on_toggle_canny);
        connect (slider_canny_thr1, &QSlider::valueChanged, this, &main_window::on_slider_canny1);
        connect (slider_canny_thr2, &QSlider::valueChanged, this, &main_window::on_slider_canny2);
    }

    /// shake
    {
        auto * box = new QGroupBox ("Shake");
        auto * h = new QHBoxLayout (box);
        chk_shake_ = new QCheckBox ("Enable", box);
        chk_shake_->setChecked (false);
        h->addWidget (chk_shake_);
        box->setLayout (h);
        v->addWidget (box);
        connect (chk_shake_, &QCheckBox::toggled, this, &main_window::on_toggle_shake);
    }

    /// sort
    {
        auto * box = new QGroupBox ("Sort");
        auto * h = new QVBoxLayout (box);
        chk_sort_ = new QCheckBox ("Enable", box);
        chk_sort_->setChecked (false);
        h->addWidget (chk_sort_);

        combo_sort_mode_ = new QComboBox (box);
        combo_sort_mode_->addItem ("Luminosity", QVariant::fromValue (sort_mode::Luminosity));
        combo_sort_mode_->addItem ("Red",        QVariant::fromValue (sort_mode::Red));
        combo_sort_mode_->addItem ("Green",      QVariant::fromValue (sort_mode::Green));
        combo_sort_mode_->addItem ("Blue",       QVariant::fromValue (sort_mode::Blue));
        combo_sort_mode_->addItem ("Hue",        QVariant::fromValue (sort_mode::Hue));
        h->addWidget (combo_sort_mode_);

        combo_sort_scope_ = new QComboBox (box);
        combo_sort_scope_->addItem ("Global", QVariant::fromValue (sort_scope::Global));
        combo_sort_scope_->addItem ("Rows", QVariant::fromValue (sort_scope::Rows));
        combo_sort_scope_->addItem ("Cols", QVariant::fromValue (sort_scope::Cols));
        h->addWidget (combo_sort_scope_);

        combo_sort_axis_ = new QComboBox (box);
        combo_sort_axis_->addItem ("Horizontal", QVariant::fromValue (sort_axis::Horizontal));
        combo_sort_axis_->addItem ("Vertical", QVariant::fromValue (sort_axis::Vertical));
        h->addWidget (combo_sort_axis_);

        auto * layout_chunk = new QHBoxLayout ();
        layout_chunk->addWidget (new QLabel ("Chunk size :", box));
        spin_sort_chunk_ = new QSpinBox (box);
        spin_sort_chunk_->setRange (0, 300);
        spin_sort_chunk_->setValue (0);
        layout_chunk->addWidget (spin_sort_chunk_);
        h->addLayout (layout_chunk);

        auto * layout_stride = new QHBoxLayout ();
        layout_stride->addWidget (new QLabel ("Stride size :", box));
        spin_sort_stride_ = new QSpinBox (box);
        spin_sort_stride_->setRange (0, 300);
        spin_sort_stride_->setValue (0);
        layout_stride->addWidget (spin_sort_stride_);
        h->addLayout (layout_stride);

        box->setLayout (h);
        v->addWidget (box);
        connect (chk_sort_, &QCheckBox::toggled, this, &main_window::on_toggle_sort);
        connect (combo_sort_mode_, QOverload<int>::of (&QComboBox::currentIndexChanged), this, &main_window::on_combo_sort_mode);
        connect (combo_sort_scope_, QOverload<int>::of (&QComboBox::currentIndexChanged), this, &main_window::on_combo_sort_scope);
        connect (combo_sort_axis_, QOverload<int>::of (&QComboBox::currentIndexChanged), this, &main_window::on_combo_sort_axis);
        connect (spin_sort_chunk_, QOverload<int>::of (&QSpinBox::valueChanged), this, &main_window::on_spin_sort_chunk);
        connect (spin_sort_stride_, QOverload<int>::of (&QSpinBox::valueChanged), this, &main_window::on_spin_sort_stride);
    }

    /// kuwahara
    {
        auto * box = new QGroupBox ("Kuwahara");
        auto * h = new QVBoxLayout (box);
        chk_kuwahara_ = new QCheckBox ("Enable", box);
        chk_kuwahara_->setChecked (false);
        h->addWidget (chk_kuwahara_);

        auto * layout_k = new QHBoxLayout ();
        layout_k->addWidget (new QLabel ("Window size :", box));
        spin_kuwahara_ = new QSpinBox (box);
        spin_kuwahara_->setRange (3, 21);
        spin_kuwahara_->setValue (3);
        layout_k->addWidget (spin_kuwahara_);
        h->addLayout (layout_k);

        box->setLayout (h);
        v->addWidget (box);
        connect (chk_kuwahara_, &QCheckBox::toggled, this, &main_window::on_toggle_kuwahara);
        connect (spin_kuwahara_, QOverload<int>::of (&QSpinBox::valueChanged), this, &main_window::on_spin_kuwahara);
    }

    v->addStretch (1);
    w->setLayout (v);
    scroll->setWidget (w);
    dock_filters_->setWidget (scroll);
    addDockWidget (Qt::RightDockWidgetArea, dock_filters_);
}


void main_window::create_menus ()
{
    auto * file_menu = menuBar ()->addMenu ("File");
    auto * act_show_test = new QAction ("Show Test Image", this);
    connect (act_show_test, &QAction::triggered, this, &main_window::show_test_image);
    file_menu->addAction (act_show_test);

    auto * act_open_image = new QAction ("Open Image", this);
    connect (act_open_image, &QAction::triggered, this, &main_window::open_image_from_disk);
    file_menu->addAction (act_open_image);

    auto * act_open_video = new QAction ("Open Video", this);
    connect (act_open_video, &QAction::triggered, this, &main_window::open_video_file);
    file_menu->addAction (act_open_video);

    auto * source_menu = menuBar ()->addMenu ("Source");
    act_start_webcam_ = new QAction ("Start Webcam", this);
    connect (act_start_webcam_, &QAction::triggered, this, &main_window::start_webcam);
    source_menu->addAction (act_start_webcam_);

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
    stop_source ();
    cv::Mat mat = utils::load_image_mat (":/test.png");
    if (mat.empty ())
    {
        preview_label_->setText ("Failed to load :/test.jpg");
        statusBar ()->showMessage ("Failed to load test image", 3000);
        return;
    }
    is_live_ = false;

    current_frame_orig_ = mat;
    reprocess_and_show ();

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
    is_live_ = false;

    current_frame_orig_ = mat;
    reprocess_and_show ();
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
    is_live_ = true;

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

    act_start_webcam_->setEnabled (false);
    act_stop_->setEnabled (true);
    grab_timer_->start ();

    preview_label_->setText ("Starting Webcam");
    statusBar()->showMessage ("Started Webcam", 3000);
}

void main_window::stop_source()
{
    if (source_)
    {
        source_->close ();
        source_.reset ();
    }
    is_live_ = false;

    act_start_webcam_->setEnabled (true);
    act_stop_->setEnabled (false);

    current_pixmap_ = QPixmap ();
    preview_label_->clear ();
    preview_label_->setText ("Source stopped");
    preview_label_->setAlignment (Qt::AlignCenter);

    statusBar ()->showMessage ("Source stopped", 3000);
}

void main_window::on_grab_tick ()
{
    /// image is shown but we still need to update live filters
    if (!is_live_)
    {
        reprocess_and_show ();
        return;
    }

    if (!source_ || !source_->is_opened ())
    {
        stop_source ();
        return;
    }

    cv::Mat frame;
    if (!source_->read (frame) || frame.empty ())
    {
        if (source_->get_type () == video_source::type::Video)
        {
            source_->restart_video ();
        }
        else
        {
            stop_source ();
            preview_label_->setText ("Stopped source");
            statusBar ()->showMessage ("Failed to get frames from source", 3000);
            return;
        }
    }

    if (source_->get_type () == video_source::type::Webcam)
        cv::flip (frame, frame, 1);

    current_frame_orig_ = frame;
    reprocess_and_show ();
}

void main_window::open_video_file()
{
    stop_source ();

    const QString filter = "Video (*.mp4 *.avi *.mov);;All (*.*)";
    const QString path = QFileDialog::getOpenFileName (this, "Open Video", QString (), filter);
    if (path.isEmpty ()) return;

    source_ = std::make_unique<video_source> ();
    if (!source_->open_video (path.toStdString ()))
    {
        source_.reset ();
        preview_label_->setText (QString("Failed to open video:\n%1").arg(path));
        statusBar ()->showMessage ("Failed to open video", 3000);
        return;
    }
    is_live_ = true;

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

    act_start_webcam_->setEnabled (true);
    act_stop_->setEnabled (true);
    grab_timer_->start ();

    statusBar()->showMessage (QString("Opened Video : %1").arg (path), 3000);
}

void main_window::reprocess_and_show ()
{
    if (current_frame_orig_.empty ())
        return;

    tick_num_++;

    current_frame_processed_ = current_frame_orig_.clone ();
    frame_info fi {tick_num_};
    filter_chain_.apply_all (current_frame_processed_, fi);
    show_mat (current_frame_processed_);
}

void main_window::show_mat (const cv::Mat & mat)
{
    current_pixmap_ = utils::mat_to_pixmap (mat);
    update_preview ();
}

void main_window::on_toggle_grayscale (bool on)
{
    filter_grayscale_->set_enabled (on);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_toggle_blur (bool on)
{
    filter_blur_->set_enabled (on);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_slider_blur (int val)
{
    filter_blur_->change_k_size (val);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_toggle_canny (bool on)
{
    filter_canny_->set_enabled (on);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_slider_canny1 (double val)
{
    filter_canny_->set_thr1 (val);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_slider_canny2 (double val)
{
    filter_canny_->set_thr2 (val);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_toggle_shake (bool on)
{
    filter_shake_->set_enabled (on);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_toggle_sort (bool on)
{
    filter_sort_->set_enabled (on);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_combo_sort_mode (int idx)
{
    QVariant data = combo_sort_mode_->itemData (idx);
    filter_sort_->set_mode (data.value<sort_mode> ());
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_combo_sort_scope (int idx)
{
    QVariant data = combo_sort_scope_->itemData (idx);
    filter_sort_->set_scope (data.value<sort_scope> ());
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_combo_sort_axis (int idx)
{
    QVariant data = combo_sort_axis_->itemData (idx);
    filter_sort_->set_axis (data.value<sort_axis> ());
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_spin_sort_chunk (int val)
{
    filter_sort_->set_chunk (val);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_spin_sort_stride (int val)
{
    filter_sort_->set_stride (val);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_toggle_kuwahara (bool on)
{
    filter_kuwahara_->set_enabled (on);
    if (!is_live_) reprocess_and_show ();
}

void main_window::on_spin_kuwahara (int val)
{
    filter_kuwahara_->set_k_size (val);
    if (!is_live_) reprocess_and_show ();
}