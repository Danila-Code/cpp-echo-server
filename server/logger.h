#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string_view>

class Logger {
public:
    explicit Logger(std::ostream& output) : output_{output} {
    }
    // write str and time to output_
    void operator()(std::string_view str) {
        PrintCurrentTime();
        output_ << ' ' << str << std::endl;
    }

private:
    void PrintCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        output_ << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    }

    std::ostream& output_;
};