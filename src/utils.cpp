#include "utils.h"

#include <QFile>
#include <opencv2/imgcodecs.hpp>

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


}