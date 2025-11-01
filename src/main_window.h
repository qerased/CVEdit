#ifndef CVEDIT_MAIN_WINDOW_H
#define CVEDIT_MAIN_WINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <QCheckBox>
#include <QDockWidget>
#include <QComboBox>
#include <QSlider>

#include "video_source.h"
#include "filters/filter.h"
#include "filters/filter_blur.h"
#include "filters/filter_canny.h"
#include "filters/filter_grayscale.h"
#include "filters/filter_shake.h"
#include "filters/filter_sort.h"

class main_window : public QMainWindow
{
    Q_OBJECT
public:
    explicit main_window (QWidget *parent = nullptr);

private slots:
    void show_test_image ();
    void open_image_from_disk ();

    void start_webcam ();
    void stop_source ();
    void on_grab_tick ();

    void open_video_file ();

    /// filters
    void on_toggle_grayscale (bool on);

    void on_toggle_blur (bool on);
    void on_slider_blur (int val);

    void on_toggle_canny (bool on);
    void on_slider_canny1 (double val);
    void on_slider_canny2 (double val);

    void on_toggle_shake (bool on);

    void on_toggle_sort (bool on);
    void on_combo_sort (int idx);

private:
    void create_ui ();
    void create_menus ();
    void create_status_bar ();
    void create_filters_dock ();

    void update_preview ();

    void reprocess_and_show ();
    void show_mat (const cv::Mat & mat);

    void resizeEvent(QResizeEvent *event) override;

    QLabel * preview_label_ = nullptr;
    QPixmap current_pixmap_;

    std::unique_ptr <video_source> source_;
    QTimer * grab_timer_ = nullptr;

    QAction * act_start_webcam_ = nullptr;
    QAction * act_stop_         = nullptr;

    //// filters
    unsigned int tick_num_ {0};
    cv::Mat current_frame_orig_;
    cv::Mat current_frame_processed_;
    bool is_live_ {false};

    filter_chain filter_chain_;

    QDockWidget * dock_filters_ = nullptr;

    filter_grayscale * filter_grayscale_;
    QCheckBox * chk_grayscale_  = nullptr;

    filter_blur * filter_blur_;
    QCheckBox * chk_blur_       = nullptr;
    QSlider * slider_blur_      = nullptr;

    filter_canny * filter_canny_;
    QCheckBox * chk_canny_       = nullptr;
    QSlider * slider_canny_thr1  = nullptr;
    QSlider * slider_canny_thr2  = nullptr;

    filter_shake * filter_shake_;
    QCheckBox * chk_shake_       = nullptr;

    filter_sort * filter_sort_;
    QCheckBox * chk_sort_         = nullptr;
    QComboBox * combo_sort_       = nullptr;
};


#endif //CVEDIT_MAIN_WINDOW_H