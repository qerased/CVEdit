#include "utils.h"

#include <QFile>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace utils
{

cv::Mat load_image_mat (const QString &path, QString *error)
{
    QFile file (path);
    if (!file.open (QIODevice::ReadOnly))
    {
        if (error)
            *error = file.errorString();
        return {};
    }
    QByteArray data = file.readAll();
    file.close();

    std::vector<uchar> buf (data.begin(), data.end());
    cv::Mat img = cv::imdecode (buf, cv::IMREAD_UNCHANGED);

    if (img.empty ())
    {
        if (error)
            *error = "Error: unable to decode image with OpenCV";
        return {};
    }

    return img;
}

QImage mat_to_qimage (const cv::Mat &img)
{
    if (img.empty ())
        return {};

    switch (img.type ())
    {
        case CV_8UC1:
        {
            QImage ret (img.cols, img.rows, QImage::Format_Grayscale8);
            for (int y = 0; y < img.rows; y++)
                memcpy (ret.scanLine (y), img.ptr (y), static_cast<size_t> (img.cols));
            return ret;
        }
        case CV_8UC3:
        {
            cv::Mat rgb;
            cv::cvtColor (img, rgb, cv::COLOR_BGR2RGB);
            QImage ret (reinterpret_cast<const uchar*> (rgb.data),
                   rgb.cols, rgb.rows,
                        static_cast<int> (rgb.step),
                        QImage::Format_RGB888);
            return ret.copy ();
        }
        case CV_8UC4:
        {
            cv::Mat rgba;
            cv::cvtColor (img, rgba, cv::COLOR_BGRA2RGBA);
            QImage ret (reinterpret_cast<const uchar*> (rgba.data),
                        rgba.cols, rgba.rows,
                        static_cast<int>(rgba.step),
                        QImage::Format_RGBA8888);
            return ret.copy ();
        }
        default:
        {
            cv::Mat tmp;
            img.convertTo (tmp, CV_8U);
            if (tmp.channels() == 1)
                return mat_to_qimage (tmp);
            cv::Mat rgb;
            cv::cvtColor(tmp, rgb, cv::COLOR_BGR2RGB);
            QImage img (reinterpret_cast<const uchar*> (rgb.data),
                        rgb.cols, rgb.rows,
                        static_cast<int> (rgb.step),
                        QImage::Format_RGB888);
            return img.copy ();
        }

    }
}

}