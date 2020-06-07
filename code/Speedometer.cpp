//
// Created by flxwly on 07.06.2020.
//

#include "Speedometer.hpp"

Speedometer::Speedometer() = default;

double Speedometer::avg_speed(unsigned int entries) {
    // get the lower value to avoid out of bounds
    entries = (entries < Speedometer::dist_per_millisecond.size()) ? entries : Speedometer::dist_per_millisecond.size();

    if (entries == 0)
        return 0;

    // sum up all the last speed measurements
    double sum = 0;
    for (int i = 0; i < entries; ++i) {
        sum += Speedometer::dist_per_millisecond[i];
    }

    // divide by entries to get the avg
    return sum / entries;
}

void Speedometer::log_speed(double dist, int wheel_speed) {
    if (Speedometer::dist_per_millisecond.size() > 500)
        Speedometer::dist_per_millisecond.erase(Speedometer::dist_per_millisecond.begin());

    if (wheel_speed == 0)
        return;

    double time = std::chrono::duration_cast<std::chrono::milliseconds>(
            Speedometer::timer::now() - Speedometer::last_log).count();
    Speedometer::last_log = Speedometer::timer::now();
    double dist_per_time = dist / wheel_speed / time;

    std::cout << time << "\n";
    std::cout << dist_per_time << "\n";

    Speedometer::dist_per_millisecond.push_back(dist_per_time);
}
