
#ifndef KEYFRAME_H
#define KEYFRAME_H

class KeyFrame {
public:
    int topEyelidAngle;
    int bottomEyelidAngle;
    int horizontalAngle;
    int verticalAngle;
    unsigned long duration; // Duration in milliseconds

    KeyFrame(int top, int bottom, int horizontal, int vertical, unsigned long dur)
        : topEyelidAngle(top), bottomEyelidAngle(bottom), horizontalAngle(horizontal), verticalAngle(vertical), duration(dur) {}
};

#endif //KEYFRAME_H
