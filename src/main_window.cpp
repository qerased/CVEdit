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
#include <QToolBar>
#include <QColor>
#include <QColorDialog>
#include <opencv2/core.hpp>

#include "utils.h"

main_window::main_window (QWidget *parent)
{
    setWindowTitle ("CVEdit");
    resize (1280, 720);

    create_ui ();
    create_menus ();
    create_status_bar ();

    filter_grayscale_ = filter_chain_.add<filter_grayscale> ();
    filter_blur_ = filter_chain_.add<filter_blur> ();
    filter_shake_ = filter_chain_.add<filter_shake> ();
    filter_sort_ = filter_chain_.add<filter_sort> ();
    filter_canny_ = filter_chain_.add<filter_canny> ();
    filter_kuwahara_ = filter_chain_.add<filter_kuwahara> ();
    filter_bloom_ = filter_chain_.add<filter_bloom> ();
    filter_colorize_ = filter_chain_.add<filter_colorize> ();
    filter_vignette_ = filter_chain_.add<filter_vignette> ();
    filter_film_noise_ = filter_chain_.add<filter_film_noise> ();
    filter_scanlines_ = filter_chain_.add<filter_scanlines> ();
    filter_hue_shift_ = filter_chain_.add<filter_hue_shift> ();
    filter_pixel_displace_ = filter_chain_.add<filter_pixel_displace> ();

    create_filters_dock ();

    filter_chain_.disable_all ();
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
    /// grab_timer_->start ();
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

        h->addLayout (get_chk_ord_layout (box, filter_grayscale_, chk_grayscale_, spin_grayscale_ord_));

        box->setLayout (h);
        v->addWidget (box);
    }

    /// blur
    {
        auto * box = new QGroupBox ("Gaussian Blur");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_blur_, chk_blur_, spin_blur_ord_));

        h->addLayout (get_slider (box, "Intensity :", slider_blur_,
              5, 1, 100,
              [f = filter_blur_] (int v) {f->change_k_size (v); }));

        box->setLayout (h);
        v->addWidget (box);
    }

    /// canny
    {
        auto * box = new QGroupBox ("Canny");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_canny_, chk_canny_, spin_canny_ord_));

        h->addLayout (get_checkbox (box, "Replace :", chk_canny_replace_,
            [f = filter_canny_] (bool on) { f->set_replace (on); }));

        h->addLayout (get_slider (box, "Threshold 1:", slider_canny_thr1,
            100, 1, 200,
            [f = filter_canny_] (int v){ f->set_thr1 (v); }));

        h->addLayout (get_slider (box, "Threshold 2:", slider_canny_thr2,
            200, 1, 200,
            [f = filter_canny_] (int v){ f->set_thr2 (v); }));

        box->setLayout (h);
        v->addWidget (box);
    }

    /// shake
    {
        auto * box = new QGroupBox ("Shake");
        auto * h = new QHBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_shake_, chk_shake_, spin_shake_ord_));
        box->setLayout (h);
        v->addWidget (box);
    }

    /// sort
    {
        auto * box = new QGroupBox ("Sort");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_sort_, chk_sort_, spin_sort_ord_));

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

        h->addLayout (get_spin (box, "Chunk size:", spin_sort_chunk_,
            0, 0, 300,
            [f = filter_sort_] (int v) { f->set_chunk (v); }));

        h->addLayout (get_spin (box, "Stride size:", spin_sort_stride_,
            0, 0, 300,
            [f = filter_sort_] (int v) { f->set_stride (v); }));

        h->addLayout (get_checkbox (box, "Use Random Mask:", chk_sort_mask_,
            [f = filter_sort_] (bool on) { f->set_random_mask_enabled(on); }));

        h->addLayout (get_slider (box, "Mask probability:", slider_sort_mask_prob_,
            0, 0, 100,
            [f = filter_sort_] (int v){ f->set_mask_prob (v / 100.); }));

        h->addLayout (get_checkbox (box, "Enable Threshold:", chk_sort_thr_enabled_,
            [f = filter_sort_] (bool on) { f->set_threshold_enabled (on); }));

        h->addLayout (get_slider (box, "Threshold low:", slider_sort_thr_lo_,
            40, 0, 255,
            [f = filter_sort_] (int v){ f->set_threshold_lo (v); }));

        h->addLayout (get_slider (box, "Threshold high:", slider_sort_thr_hi_,
            190, 0, 255,
            [f = filter_sort_] (int v) { f->set_threshold_hi (v); }));

        h->addLayout (get_checkbox (box, "Enable random chunk:", chk_sort_rand_chunk_,
            [f = filter_sort_] (bool on) { f->enable_rand_chunk (on); }));

        box->setLayout (h);
        v->addWidget (box);
        bind_combo (combo_sort_mode_, [f = filter_sort_] (const QVariant & v) { f->set_mode (v.value<sort_mode> ()); });
        bind_combo (combo_sort_scope_, [f = filter_sort_] (const QVariant & v) { f->set_scope (v.value<sort_scope> ()); });
        bind_combo (combo_sort_axis_, [f = filter_sort_] (const QVariant & v) { f->set_axis (v.value<sort_axis> ()); });
    }

    /// kuwahara
    {
        auto * box = new QGroupBox ("Kuwahara");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_kuwahara_, chk_kuwahara_, spin_kuwahara_ord_));

        h->addLayout (get_spin (box, "Window size :", spin_kuwahara_,
            3, 3, 21,
            [f = filter_kuwahara_] (int v) { f->set_k_size (v); }));

        box->setLayout (h);
        v->addWidget (box);
    }

    /// edgeglow
    {
        auto * box = new QGroupBox ("Bloom");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_bloom_, chk_bloom_, spin_bloom_ord_));
        h->addLayout (get_slider (box, "Threshold :", slider_bloom_thresh_, 70, 0, 100,
            [f = filter_bloom_] (int v) { f->set_thresh (v / 100.); }));

        h->addLayout (get_slider (box, "Radius :", slider_bloom_radius_, 10, 1, 30,
            [f = filter_bloom_] (int v) { f->set_radius (v); }));

        h->addLayout (get_slider (box, "Strength :", slider_bloom_coeff_, 80, 0, 100,
            [f = filter_bloom_] (int v) { f->set_coeff (v / 100.); }));

        auto * layout = new QVBoxLayout ();
        btn_bloom_color_ = new QPushButton ("Select Color");
        label_bloom_color_ = new QLabel ("No color selected");
        label_bloom_color_->setAlignment (Qt::AlignCenter);

        layout->addWidget (btn_bloom_color_);
        layout->addWidget (label_bloom_color_);

        connect (btn_bloom_color_, &QPushButton::clicked, [&]()
        {
            QColor col = QColorDialog::getColor (Qt::white, this, "Color selection");
            if (col.isValid ())
            {
                filter_bloom_->set_bloom_color (cv::Scalar{col.blue () / 255., col.green () / 255., col.red () / 255.});

                QString style = QString("background-color: %1;").arg (col.name ());
                label_bloom_color_->setText (col.name ());
                label_bloom_color_->setStyleSheet (style);

                if (!is_live_) reprocess_and_show ();
            }
        });

        h->addLayout (layout);

        box->setLayout (h);
        v->addWidget (box);
    }

    /// colorize
    {
        auto * box = new QGroupBox ("Tint");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_colorize_, chk_colorize_, spin_colorize_ord_));

        auto * layout = new QVBoxLayout ();
        btn_colorize_color_ = new QPushButton ("Select Color");
        label_colorize_color_ = new QLabel ("No color selected");
        label_colorize_color_->setAlignment (Qt::AlignCenter);

        layout->addWidget (btn_colorize_color_);
        layout->addWidget (label_colorize_color_);

        connect (btn_colorize_color_, &QPushButton::clicked, [&]()
        {
            QColor col = QColorDialog::getColor (Qt::white, this, "Color selection");
            if (col.isValid ())
            {
                filter_colorize_->set_tint_color (cv::Scalar{col.blue () / 1., col.green () / 1., col.red () / 1.});

                QString style = QString("background-color: %1;").arg (col.name ());
                label_colorize_color_->setText (col.name ());
                label_colorize_color_->setStyleSheet (style);

                if (!is_live_) reprocess_and_show ();
            }
        });

        layout->addLayout (get_slider (box, "Strength:", slider_colorize_str_,
            0, 0, 100,
            [f = filter_colorize_] (int v) { f->set_strength (v / 100.); }));

        h->addLayout (layout);

        box->setLayout (h);
        v->addWidget (box);
    }

    /// vignette
    {
        auto * box = new QGroupBox ("Vignette");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_vignette_, chk_vignette_, spin_vignette_ord_));

        h->addLayout (get_slider (box, "Strength:", slider_vignette_str_,
            50, 0, 100,
            [f = filter_vignette_] (int v) { f->set_strength (v / 100.); }));

        h->addLayout (get_slider (box, "Radius:", slider_vignette_radius_,
            5, 0, 100,
            [f = filter_vignette_] (int v) { f->set_radius (v / 100.); }));

        box->setLayout (h);
        v->addWidget (box);
    }

    /// filn_noise
    {
        auto * box = new QGroupBox ("Film Noise");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_film_noise_, chk_film_noise_, spin_film_noise_ord_));

        h->addLayout (get_slider (box, "Grain:", slider_film_noise_grain_,
            25, 0, 100,
            [f = filter_film_noise_] (int v) { f->set_grain (v / 100.); }));

        h->addLayout (get_slider (box, "Scratches:", slider_film_noise_scrathes_,
            15, 0, 100,
            [f = filter_film_noise_] (int v) { f->set_scratches (v / 100.); }));

        h->addLayout (get_slider (box, "Flicker:", slider_film_noise_flicker_,
            6, 0, 100,
            [f = filter_film_noise_] (int v) { f->set_flicker (v / 100.); }));

        box->setLayout (h);
        v->addWidget (box);
    }

    /// scanlines
    {
        auto * box = new QGroupBox ("Scanlines");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_scanlines_, chk_scanlines_, spin_scanlines_ord_));

        h->addLayout (get_slider (box, "Darkness:", slider_scanlines_darkness_,
            25, 0, 100,
            [f = filter_scanlines_] (int v) { f->set_darkness (v / 100.); }));

        h->addLayout (get_slider (box, "Density:", slider_scanlines_density_,
            2, 1, 10,
            [f = filter_scanlines_] (int v) { f->set_density (v); }));

        h->addLayout (get_slider (box, "Speed:", slider_scanlines_speed_,
            0, 0, 100,
            [f = filter_scanlines_] (int v) { f->set_speed (v); }));

        box->setLayout (h);
        v->addWidget (box);
    }

    /// hue shift
    {
        auto * box = new QGroupBox ("Hue Shift");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_hue_shift_, chk_hue_shift_, spin_hue_shift_ord_));

        h->addLayout (get_slider (box, "Speed:", slider_hue_shift_speed_,
            12, 0, 100,
            [f = filter_hue_shift_] (int v) { f->set_speed (v / 100.); }));

        box->setLayout (h);
        v->addWidget (box);
    }

    /// pixel displace
    {
        auto * box = new QGroupBox ("Pixel Displace");
        auto * h = new QVBoxLayout (box);
        h->addLayout (get_chk_ord_layout (box, filter_pixel_displace_, chk_pixel_displace_, spin_pixel_displace_ord_));

        h->addLayout (get_spin (box, "Block Height:", spin_pixel_displace_bh_,
            16, 1, 100,
            [f = filter_pixel_displace_] (int v) { f->set_block_h (v); }));

        h->addLayout (get_spin (box, "Block Width:", spin_pixel_displace_bw_,
            32, 1, 100,
            [f = filter_pixel_displace_] (int v) { f->set_block_w (v); }));

        h->addLayout (get_spin (box, "Max Shift:", spin_pixel_displace_shift_,
            12, 1, 100,
            [f = filter_pixel_displace_] (int v) { f->set_max_shift (v); }));

        h->addLayout (get_slider (box, "Speed:", spin_pixel_displace_speed_,
            4, 1, 100,
            [f = filter_pixel_displace_] (int v) { f->set_speed (v); }));

        box->setLayout (h);
        v->addWidget (box);
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

    auto * tb = addToolBar ("Main");
    tb->setObjectName ("tbMain");
    act_screenshot_ = new QAction ("Screenshot", this);
    connect (act_screenshot_, &QAction::triggered, this, &main_window::save_screenshot);
    tb->addAction (act_screenshot_);
}

void main_window::save_screenshot ()
{
    if (current_pixmap_.isNull ())
    {
        statusBar ()->showMessage ("No image to save", 3000);
        return;
    }

    const QString sugg = QDir::homePath () + "/screenshot_" +
        QDateTime::currentDateTime ().toString ("yyyyMMdd_HHmmss") + ".png";

    const QString path = QFileDialog::getSaveFileName (
        this, "Save screenshot", sugg, "PNG (*.png);;JPEG (*.jpg *.jpeg)");

    if (path.isEmpty ())
        return;

    if (!current_pixmap_.save (path))
    {
        statusBar ()->showMessage ("Cannot save screenshot", 3000);
        return;
    }

    statusBar ()->showMessage (QString ("Saved: %1").arg (path), 3000);
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
        /// reprocess_and_show ();
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