#ifndef CVEDIT_UTILS_H
#define CVEDIT_UTILS_H

#include <QString>
#include <opencv2/core.hpp>

namespace utils
{

cv::Mat load_image_mat (const QString & path, QString * error = nullptr);

}

#endif //CVEDIT_UTILS_H