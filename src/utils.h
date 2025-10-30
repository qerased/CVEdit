#ifndef CVEDIT_UTILS_H
#define CVEDIT_UTILS_H

#include <qpixmap.h>
#include <QString>
#include <opencv2/core.hpp>

namespace utils
{

cv::Mat load_image_mat (const QString & path, QString * error = nullptr);

QImage mat_to_qimage (const cv::Mat & img);

inline QPixmap mat_to_pixmap (const cv::Mat & img)
{
    return QPixmap::fromImage (mat_to_qimage (img));
}

}

#endif //CVEDIT_UTILS_H