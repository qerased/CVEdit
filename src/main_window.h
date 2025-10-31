#ifndef CVEDIT_MAIN_WINDOW_H
#define CVEDIT_MAIN_WINDOW_H
#include <QMainWindow>
#include <QLabel>

#include "video_source.h"
#include "filters/filter.h"
#include "filters/filter_grayscale.h"

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

private:
    void create_ui ();
    void create_menus ();
    void create_status_bar ();

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
    cv::Mat current_frame_orig_;
    cv::Mat current_frame_processed_;
    bool is_live_ {false};

    filter_chain filter_chain_;
    filter_grayscale * filter_grayscale_;
};


#endif //CVEDIT_MAIN_WINDOW_H