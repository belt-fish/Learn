#include <opencv2/opencv.hpp>

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

cv::Rect makeRoi(const cv::Mat& img) {
    const int w = img.cols;
    const int h = img.rows;
    const int roiw = w / 2;
    const int roih = h / 4;
    const int x = w / 2;
    const int y = h / 2;
    return cv::Rect(x, y, roiw, roih);
}

cv::Rect clampRoi(const cv::Rect& r, const cv::Mat& img) {
    const int x = std::max(0, r.x);
    const int y = std::max(0, r.y);
    const int w = std::min(r.width, img.cols - x);
    const int h = std::min(r.height, img.rows - y);
    return cv::Rect(x, y, std::max(0, w), std::max(0, h));
}

}

int main(int argc, char** argv) {
    std::string path = "assets/sample.jpg";
    bool show_window = true;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--no-window") {
            show_window = false;
        } else {
            path = arg;
        }
    }

    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "[错误] 读不到图片：" << path << "\n"
                  << "       检查路径对不对，或者文件是不是真的图片\n";
        return 1;
    }
    std::cout << "读图成功：" << path << "\n";
    std::cout << "    宽 " << img.cols << "，高 " << img.rows
              << "，通道数 " << img.channels() << "\n";

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    std::cout << "已转灰度，通道数 " << gray.channels() << "\n";

    cv::Rect roi = clampRoi(makeRoi(img), img);
    if (roi.width <= 0 || roi.height <= 0) {
        std::cerr << "[错误] 裁剪区域无效\n";
        return 1;
    }
    cv::Mat crop = img(roi).clone();

    cv::Mat with_box = img.clone();
    cv::rectangle(with_box, roi, cv::Scalar(0, 0, 255), 2);
    cv::putText(with_box, "ROI", cv::Point(roi.x, std::max(20, roi.y - 8)),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);

    std::cout << "ROI: x=" << roi.x << " y=" << roi.y
              << " w=" << roi.width << " h=" << roi.height << "\n";

    if (show_window) {
        cv::imshow("1 original (ROI marked)", with_box);
        cv::imshow("2 gray", gray);
        cv::imshow("3 roi", crop);
        std::cout << "按任意键关闭\n";
        cv::waitKey(0);
        cv::destroyAllWindows();
    } else {
        std::cout << "跳过显示\n";
    }

    std::string out_dir = "output";
    std::filesystem::create_directories(out_dir);

    const std::string gray_path = out_dir + "/gray.jpg";
    const std::string roi_path = out_dir + "/roi.jpg";
    const std::string box_path = out_dir + "/roi_marked.jpg";

    const bool ok_gray = cv::imwrite(gray_path, gray);
    const bool ok_roi = cv::imwrite(roi_path, crop);
    const bool ok_box = cv::imwrite(box_path, with_box);

    if (!ok_gray || !ok_roi || !ok_box) {
        std::cerr << "有图片保存失败\n";
        return 1;
    }
    std::cout << "保存成功\n";
    return 0;
}
