#include <opencv2/opencv.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct LightBar {
    cv::Rect box;
    cv::Point2f center;
    float width = 0;
    float height = 0;
    float area = 0;
};

struct Armor {
    cv::Rect box;
    int left = -1;
    int right = -1;
};

const cv::Scalar kLowerBlue(90, 100, 200);
const cv::Scalar kUpperBlue(130, 255, 255);

cv::Mat buildMask(const cv::Mat& bgr) {
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    cv::Mat mask;
    cv::inRange(hsv, kLowerBlue, kUpperBlue, mask);

    const cv::Mat k = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, k);
    return mask;
}

std::vector<LightBar> findBars(const cv::Mat& mask) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<LightBar> bars;
    for (const auto& c : contours) {
        const float area = static_cast<float>(cv::contourArea(c));
        if (area < 50.0f) continue;

        const cv::Rect r = cv::boundingRect(c);
        if (r.width < 2 || r.height < 3) continue;

        const float aspect = static_cast<float>(r.height) / static_cast<float>(r.width);
        if (aspect < 1.8f) continue;

        LightBar b;
        b.box = r;
        b.center = cv::Point2f(r.x + r.width * 0.5f, r.y + r.height * 0.5f);
        b.width = static_cast<float>(r.width);
        b.height = static_cast<float>(r.height);
        b.area = area;
        bars.push_back(b);
    }

    std::sort(bars.begin(), bars.end(),
              [](const LightBar& a, const LightBar& b) { return a.center.x < b.center.x; });
    return bars;
}

std::vector<LightBar> mergeBars(const std::vector<LightBar>& bars) {
    std::vector<LightBar> out;
    for (const auto& b : bars) {
        bool merged = false;
        for (auto& m : out) {
            const float dx = std::abs(m.center.x - b.center.x);
            const float dy = std::abs(m.center.y - b.center.y);
            if (dx < 1.2f * (m.width + b.width) * 0.5f &&
                dy < 0.8f * std::max(m.height, b.height)) {

                m.box = m.box | b.box;
                m.center = cv::Point2f(m.box.x + m.box.width * 0.5f,
                                       m.box.y + m.box.height * 0.5f);
                m.width = static_cast<float>(m.box.width);
                m.height = static_cast<float>(m.box.height);
                m.area += b.area;
                merged = true;
                break;
            }
        }
        if (!merged) out.push_back(b);
    }
    std::sort(out.begin(), out.end(),
              [](const LightBar& a, const LightBar& b) { return a.center.x < b.center.x; });
    return out;
}

std::vector<Armor> pairBars(const std::vector<LightBar>& bars) {
    std::vector<Armor> armors;
    for (size_t i = 0; i < bars.size(); ++i) {
        for (size_t j = i + 1; j < bars.size(); ++j) {
            const auto& a = bars[i];
            const auto& b = bars[j];

            const float dx = std::abs(a.center.x - b.center.x);
            if (dx < 1.0f) continue;

            const float h_ratio = std::max(a.height, b.height) / std::min(a.height, b.height);
            if (h_ratio > 1.35f) continue;

            if (std::abs(a.center.y - b.center.y) > 0.5f * std::max(a.height, b.height)) continue;

            const float avg_w = (a.width + b.width) * 0.5f;
            if (dx < 1.5f * avg_w || dx > 6.0f * avg_w) continue;

            Armor arm;
            arm.left = static_cast<int>(i);
            arm.right = static_cast<int>(j);
            arm.box = a.box | b.box;
            armors.push_back(arm);
        }
    }
    return armors;
}

}

int main(int argc, char** argv) {
    std::string path = "assets/armor.png";
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
        std::cerr << "读不到图片：" << path << "\n";
        return 1;
    }
    std::cout << "读图：" << path << "  " << img.cols << "x" << img.rows << "\n";

    cv::Mat mask = buildMask(img);

    auto bars = mergeBars(findBars(mask));
    std::cout << "找到灯条 " << bars.size() << " 根\n";
    for (size_t i = 0; i < bars.size(); ++i) {
        std::cout << "  灯条" << i << " 中心(" << bars[i].center.x << ", " << bars[i].center.y
                  << ") 宽" << bars[i].width << " 高" << bars[i].height << "\n";
    }

    auto armors = pairBars(bars);
    std::cout << "配对成功 " << armors.size() << " 块装甲板\n";

    cv::Mat result = img.clone();
    for (const auto& b : bars) {
        cv::rectangle(result, b.box, cv::Scalar(0, 255, 255), 1);
    }
    for (size_t i = 0; i < armors.size(); ++i) {
        cv::rectangle(result, armors[i].box, cv::Scalar(0, 255, 0), 2);
    }
    cv::putText(result, "Armor detected: " + std::to_string(armors.size()),
                cv::Point(10, 28), cv::FONT_HERSHEY_SIMPLEX, 0.8,
                cv::Scalar(0, 255, 255), 2);

    std::filesystem::create_directories("output");
    cv::imwrite("output/armor_mask.jpg", mask);
    cv::imwrite("output/armor_result.jpg", result);
    std::cout << "已保存 output/armor_mask.jpg 和 output/armor_result.jpg\n";

    if (show_window) {
        cv::imshow("1 original", img);
        cv::imshow("2 blue mask", mask);
        cv::imshow("3 result", result);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }
    return 0;
}
