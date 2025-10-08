#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

cv::Mat chromatic_abberation(const cv::Mat &rgb_image) {
    if (rgb_image.empty()) {
        std::cerr << "Failed to find image" << std::endl;
    }
    // debug, show the initial image
    // cv::imshow("DEBUG", rgb_image);
    //
    // rgb to hsv
    cv::Mat hsv_image;
    cv::cvtColor(rgb_image, hsv_image, cv::COLOR_RGB2HSV);
    std::vector<cv::Mat> channels(hsv_image.channels());
    // split the channels
    cv::split(hsv_image, channels);

    //desaturate
    for (int i = 0; i < hsv_image.channels(); i++) {
        channels[1] *= 0.9;
    }

    //remerge
    cv::Mat rgb_image_desaturated;
    cv::merge(channels, rgb_image_desaturated);
    cv::cvtColor(rgb_image_desaturated, rgb_image, cv::COLOR_HSV2RGB);

    //shift channel
    std::vector<cv::Mat> hue_channels(rgb_image_desaturated.channels());
    cv::split(rgb_image, hue_channels);

    int tx = 9.0f;
    cv::Mat M = (cv::Mat_<float>(2, 3) << 1, 0, tx,
                 0, 1, 0);
    int width = hue_channels[2].cols;
    int height = hue_channels[2].rows;
    cv::Mat translated_image;

    cv::warpAffine(hue_channels[2], translated_image, M, cv::Size(width, height));
    hue_channels[2] = translated_image;
    cv::merge(hue_channels, rgb_image);

    return rgb_image;
}


cv::Mat gaussian_blur(const cv::Mat &image) {
    if (image.empty()) {
        std::cerr << "Failed to find image" << std::endl;
    }
    cv::Mat blurred;
    cv::bilateralFilter(image, blurred, cv::BORDER_CONSTANT, 255, 0);
    return blurred;
}

cv::Mat luma_blur(const cv::Mat &image) {
    if (image.empty()) {
        std::cerr << "Failed to find image" << std::endl;
    }

    cv::Mat luma;
    // convert 2 lab
    cv::cvtColor(image, luma, cv::COLOR_RGB2Lab);
    std::vector<cv::Mat> luma_channels(luma.channels());
    cv::split(luma, luma_channels);
    cv::Mat blurred;
    // apply blur
    cv::GaussianBlur(luma_channels[0], blurred, cv::Size(3, 9), 0);
    // convert back 2 rgb
    luma_channels[0] = blurred;
    cv::merge(luma_channels, blurred);
    cv::cvtColor(blurred, image, cv::COLOR_Lab2RGB);
    return image;
}


cv::Mat noise(const cv::Mat &image) {
    if (image.empty()) {
        std::cerr << "Failed to find image" << std::endl;
    }
    int width = image.cols;
    int height = image.rows;
    int min = -30;
    int max = 30;
    cv::Mat noised_image;
    cv::Mat uniform_noise = cv::Mat::zeros(height, width, image.type());
    cv::randu(uniform_noise, min, max);
    cv::add(image, uniform_noise, noised_image);
    return noised_image;
}

cv::Mat jitter(const cv::Mat &image) {
    if (image.empty()) {
        std::cerr << "Failed to find image" << std::endl;
    }

    int width = image.cols;
    int height = image.rows;
    cv::Mat jittered;
    image.copyTo(jittered);
    //jittering

    int y0 = rand() % (image.rows - 20);
    int h = rand() % 60 + 20;
    int offset = rand() % 40 - 20;
    cv::Rect band_rect(0, y0, image.cols, h);
    cv::Mat band = image(band_rect);
    cv::Mat M = (cv::Mat_<float>(2, 3) << 1, 0, offset, 0, 1, 0);
    cv::warpAffine(band, band, M, band.size(),
                   cv::INTER_LINEAR, cv::BORDER_REPLICATE);
    return jittered;
}

cv::Mat glow_edges(const cv::Mat &image) {
    if (image.empty()) {
        std::cerr << "Failed to find image" << std::endl;
    }
    cv::Mat glow;
    cv::GaussianBlur(image, glow, cv::Size(9, 1), 4);
    cv::addWeighted(image, 1.0, glow, 0.25, 0, image); //halo
    return glow;
}


/** this function makes it look cartoony, might reimplement if i find a better way

// cv::Mat interlace(const cv::Mat &image) {
//     if (image.empty()) {
//         std::cerr << "Failed to find image" << std::endl;
//     }
//     int width = image.cols;
//     int height = image.rows;
//     for (int y = 0; y < height; y += 2) {
//         image.row(y) *= 0.8;
//     }
//     return image;
//
 **/


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image>" << std::endl;
    }
    cv::Mat image = cv::imread(argv[1], cv::COLOR_BGR2RGB);
    if (image.empty()) {
        std::cerr << "Failed to load image" << std::endl;
    }
    cv::Mat frame = image.clone();
    frame = chromatic_abberation(frame);
    frame = gaussian_blur(frame);
    frame = luma_blur(frame);
    frame = noise(frame);
    frame = jitter(frame);
    frame = glow_edges(frame);

    cv::imshow("Full", frame);

    cv::waitKey(0);
}
