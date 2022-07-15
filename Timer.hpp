//
//  Timer.hpp
//  RGAssignment6
//
//  Created by rick gessner on 5/17/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Timer_h
#define Timer_h
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>


class Timer {
protected:
	using systemClock_t = std::chrono::system_clock;
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t>  started;
public:

	Timer() : started(clock_t::now()) {}

	void reset() {
		started = clock_t::now();
	}

	double elapsed() const {
		return std::chrono::duration_cast<second_t>(clock_t::now() - started).count();
	}

	std::string getCurrentTime() {
		auto today = systemClock_t::now();
		// change to type time_t
		time_t today_tm = systemClock_t::to_time_t(today);

		std::stringstream ss;
		ss << std::put_time(std::localtime(&today_tm), "%F %X");
		return ss.str();
	}

};

#endif /* Timer_h */
