#ifndef CVEDIT_MAIN_WINDOW_H
#define CVEDIT_MAIN_WINDOW_H
#include <QMainWindow>
#include <QLabel>

#include "video_source.h"

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

    void resizeEvent(QResizeEvent *event) override;

    QLabel * preview_label_ = nullptr;
    QPixmap current_pixmap_;

    std::unique_ptr <video_source> source_;
    QTimer * grab_timer_ = nullptr;

    QAction * act_start_webcam = nullptr;
    QAction * act_stop_        = nullptr;
};


#endif //CVEDIT_MAIN_WINDOW_H