#include "Time.h"

double Time::delta_ = 0.01; // Initialize to non-zero value
double Time::fps_ = 60.0;	// Same, just in case
double Time::lastFrameTime_ = glfwGetTime();

double Time::deltas_[FPS_FRAMES_COUNT];
double Time::deltaSum_ = 0.0;
unsigned int Time::deltaIndex_ = 0;

// Zero out delta buffer
void Time::init() {
    std::memset(static_cast<void*>(Time::deltas_), 0, static_cast<size_t>(FPS_FRAMES_COUNT));
}

void Time::update() {
    // Calculate new time delta
    Time::delta_ = glfwGetTime() - Time::lastFrameTime_;
    Time::lastFrameTime_ = glfwGetTime();

    // Update delta buffer
    Time::deltaIndex_ = ++Time::deltaIndex_ % FPS_FRAMES_COUNT;
    Time::deltaSum_ -= Time::deltas_[Time::deltaIndex_];
    Time::deltas_[Time::deltaIndex_] = Time::delta_;
    Time::deltaSum_ += Time::deltas_[Time::deltaIndex_];

    // Calculate new FPS
    Time::fps_ = FPS_FRAMES_COUNT / Time::deltaSum_;
}