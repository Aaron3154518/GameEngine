#ifndef ANIMATION_DATA_H
#define ANIMATION_DATA_H

#include <string>

struct AnimationData {
    std::string file = "";
    unsigned int num_frames = 1, frame_ms = 100;
};

#endif
