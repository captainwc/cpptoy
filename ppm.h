#include "lc.h"

/// MARK: ppm img

struct color {
    int r;
    int g;
    int b;
};

namespace Colors {
const color BLACK{0, 0, 0};
const color WHITE{255, 255, 255};
const color RED{255, 0, 0};
const color GREEN{0, 255, 0};
const color BLUE{0, 0, 255};

inline color map_int_to_color(int x) {
    if (x < 20) {
        return WHITE;
    }
    if (x < 40) {
        return GREEN;
    }
    if (x < 60) {
        return BLUE;
    }
    if (x < 80) {
        return RED;
    }
    return BLACK;
}
}  // namespace Colors

using namespace Colors;

class Image {
public:
    explicit Image(string &&filename = "lc_ppm_demo", int width = 100, int height = 100, string &&folder = "pic")
        : img_(height, vector<color>(width, GREEN)),
          picname_(std::move(filename)),
          width_(width),
          height_(height),
          save_loc_(get_path(folder)) {
        auto r = picname_.find('.');
        if (r != string::npos) {
            picname_ = picname_.substr(0, r);
        }
    }

    explicit Image(vector<vector<color>> &&data, string &&filename = "lc_ppm_demo", string &&folder = "pic")
        : img_(std::move(data)),
          picname_(std::move(filename)),
          width_(img_[0].size()),
          height_(img_.size()),
          save_loc_(get_path(folder)) {
    }

    explicit Image(const vector<vector<int>> &data, string filename = "lc_ppm_demo",
                   const std::function<color(int)> &mapper = Colors::map_int_to_color, string &&folder = "pic")
        : img_(data.size(), vector<color>(data[0].size(), Colors::BLACK)),
          picname_(std::move(filename)),
          width_(data[0].size()),
          height_(data.size()),
          save_loc_(get_path(folder)) {
        for (int i = 0; i < height_; i++) {
            for (int j = 0; j < width_; j++) {
                img_[i][j] = mapper(data[i][j]);
            }
        }
    }

    explicit Image(const vector<int> &data, string filename = "lc_ppm_demo",
                   const std::function<color(int)> &mapper = Colors::map_int_to_color, int height = 100,
                   string &&folder = "pic")
        : img_(height, vector<color>(data.size(), Colors::BLACK)),
          picname_(std::move(filename)),
          width_(data.size()),
          height_(height),
          save_loc_(get_path(folder)) {
        int max = *max_element(data.begin(), data.end());
        if (max < height) {
            for (int j = 0; j < width_; j++) {
                for (int i = height_ - 1; i >= height_ - data[j]; i--) {
                    img_[i][j] = mapper(data[j]);
                }
            }
        } else {
            FILL_ME();
        }
    }

    Image(const Image &another)
        : img_(another.img_), picname_(another.picname_), width_(another.width_), height_(another.height_) {
    }

    Image(Image &&another) = delete;
    Image &operator=(const Image &another) = delete;
    Image &operator=(Image &&another) = delete;

    vector<color> &operator[](int row) {
        return img_[row];
    }

    void draw(string_view filetype = "jpg") {
        if (!write_to_file()) {
            cerr << "[FAILED] failed to write ppm file\n";
            return;
        }
        if (!convert(filetype)) {
            cerr << "[FAILED] failed to convert ppm file\n";
            return;
        }
        string target_file(picname_);
        target_file.append(".").append(filetype.data());
        if (!save(target_file)) {
            fprintf(stderr, "[FAILED] cannot move %s to dest\n", target_file.c_str());
        }
    }

private:
    vector<vector<color>> img_;
    string picname_;
    int width_;
    int height_;
    string save_loc_;

    static string get_path(const string &folder) {
        string ret("/mnt/d/test");
        if (folder == "") {
            return ret;
        }
        return ret.append("/").append(folder);
    }

    bool write_to_file() {
        auto ppm_file = picname_ + ".ppm";
        auto *fd = fopen(ppm_file.c_str(), "we");
        if (fd == nullptr) {
            return false;
        }
        std::atomic_bool write_finish = false;
        std::thread loader_thread([&write_finish]() {
            const char spinner[] = {'\\', '|', '/', '-'};
            int i = 0;
            while (!write_finish.load()) {
                std::cout << "\r" << spinner[i++ % 4] << " " << std::flush;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            std::cout << "\r" << std::flush;  // Clear the spinner
        });
        fprintf(fd, "P6\n%d %d 255\n", width_, height_);
        for (auto &vc : img_) {
            for (auto &c : vc) {
                fprintf(fd, "%c%c%c", c.r, c.g, c.b);
            }
        }
        fclose(fd);
        write_finish = true;
        loader_thread.join();
        return true;
    }

    bool convert(string_view filetype = "jpg") {
        auto target_file = picname_ + "." + filetype.data();
        auto convert_cmd = "convert " + picname_ + ".ppm " + target_file;
        auto rm_cmd = "rm " + picname_ + ".ppm";

        std::atomic<bool> convert_finished = false;
        std::thread loader_thread([&convert_finished]() {
            const char spinner[] = {'\\', '|', '/', '-'};
            int i = 0;
            while (!convert_finished.load()) {
                std::cout << "\r" << spinner[i++ % 4] << " " << std::flush;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            std::cout << "\r" << std::flush;  // Clear the spinner
        });

        FILE *convert_pipe = popen(convert_cmd.c_str(), "r");
        if (convert_pipe != nullptr) {
            pclose(convert_pipe);
            convert_finished = true;
            loader_thread.join();
        } else {
            loader_thread.detach();
            return false;
        }

        FILE *rm_pipe = popen(rm_cmd.c_str(), "r");
        if (rm_pipe != nullptr) {
            pclose(rm_pipe);
            return true;
        }
        return false;
    }

    bool save(string_view file) {
        if (!std::filesystem::exists(save_loc_)) {
            try {
                std::filesystem::create_directory(save_loc_);
            } catch (const std::filesystem::filesystem_error &ex) {
                cerr << ex.what() << '\n';
                return false;
            }
        }
        string mv_cmd("mv ");
        mv_cmd.append(file.data()).append(" ").append(save_loc_).append("/").append(file.data());
        auto *mv_ret = popen(mv_cmd.c_str(), "r");
        if (mv_ret != nullptr) {
            pclose(mv_ret);
            return true;
        }
        return false;
    }
};
