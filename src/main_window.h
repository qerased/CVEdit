#ifndef CVEDIT_MAIN_WINDOW_H
#define CVEDIT_MAIN_WINDOW_H
#include <QMainWindow>
#include <QLabel>

class main_window : public QMainWindow
{
    Q_OBJECT
public:
    explicit main_window (QWidget *parent = nullptr);

private slots:
    void show_test_image ();

private:
    void create_ui ();
    void create_menus ();
    void create_status_bar ();

    void update_preview ();

    void resizeEvent(QResizeEvent *event) override;

    QLabel * preview_label_ = nullptr;
    QPixmap current_pixmap_;
};


#endif //CVEDIT_MAIN_WINDOW_H