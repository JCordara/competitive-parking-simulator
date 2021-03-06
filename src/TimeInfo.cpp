#include "TimeInfo.h"

double Time::updateDelta_ = -1.0;    // Initialize to non-zero value
double Time::fps_ = 60.0;	         // Same, just in case
double Time::lastUpdateTime_ = Time::now();
double Time::timeStep_ = 1.0 / 60.0;
double Time::lag_ = Time::timeStep_; // Allows first frame to update
double Time::programStartTime_ = Time::now();

double Time::deltas_[FPS_FRAMES_COUNT];
double Time::deltaSum_ = 0.0;
unsigned int Time::deltaIndex_ = 0;

// Zero out delta buffer
void Time::init() {
    std::memset(static_cast<void*>(Time::deltas_), 0, static_cast<size_t>(FPS_FRAMES_COUNT));
}

void Time::update() {
	double now = Time::now();
    // On first call to update()
    if (Time::updateDelta_ == -1.0) {
        Time::updateDelta_ = Time::timeStep_;
        Time::lastUpdateTime_ = now;
        return;
    }

    // Calculate new time delta
    Time::updateDelta_ = now - Time::lastUpdateTime_;
    Time::lastUpdateTime_ = now;

    // Calculate accumulated lag
    Time::lag_ += Time::updateDelta_;


    // --- FPS Calculation ---
    // Update delta buffer
    Time::deltaIndex_ = ++Time::deltaIndex_ % FPS_FRAMES_COUNT;
    Time::deltaSum_ -= Time::deltas_[Time::deltaIndex_];
    Time::deltas_[Time::deltaIndex_] = Time::updateDelta_;
    Time::deltaSum_ += Time::deltas_[Time::deltaIndex_];

    // Calculate new FPS
    Time::fps_ = FPS_FRAMES_COUNT / Time::deltaSum_;
}

double Time::now() {
    std::chrono::duration<double> duration = 
        Clock::now().time_since_epoch();
    return duration.count();
}

bool Time::takeNextStep() {
    if (Time::lag_ > Time::timeStep_) {
        Time::lag_ -= Time::timeStep_;
        return true;
    } else {
        return false;
    }
}
