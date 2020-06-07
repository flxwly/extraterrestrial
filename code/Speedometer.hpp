#ifndef EXTRATERRESTRIAL_SPEEDOMETER_HPP
#define EXTRATERRESTRIAL_SPEEDOMETER_HPP

#include <vector>
#include <chrono>
#include <iostream>

class Speedometer {
public:
    Speedometer();

    double avg_speed(unsigned int entries);

    void log_speed(double dist, int wheel_speed);

private:
    typedef std::chrono::steady_clock timer;

    std::vector<double> dist_per_millisecond;
    std::chrono::time_point<std::chrono::steady_clock> last_log;
};


#endif //EXTRATERRESTRIAL_SPEEDOMETER_HPP
