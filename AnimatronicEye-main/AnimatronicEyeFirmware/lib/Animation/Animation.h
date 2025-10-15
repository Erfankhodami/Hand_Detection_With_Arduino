#include <ESP32Servo.h>
#include "KeyFrame.h"

#ifndef ANIMATION_H
#define ANIMATION_H

class Animation {
private:
    KeyFrame* keyframes;       // Pointer to an array of keyframes
    int numKeyFrames;          // Number of keyframes
    int currentKeyFrame;       // Index of the current keyframe
    unsigned long startTime;   // Time when the current keyframe started

public:
    Animation(KeyFrame* frames, int numFrames)
        : keyframes(frames), numKeyFrames(numFrames), currentKeyFrame(0), startTime(0) {}

    void start() {
        currentKeyFrame = 0;
        startTime = millis();
    }

    bool update(Servo& topEyelid, Servo& bottomEyelid, Servo& horizontal, Servo& vertical) {
        if (currentKeyFrame >= numKeyFrames) {
            return false; // Animation finished
        }

        KeyFrame currentFrame = keyframes[currentKeyFrame];
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - startTime;

        if (elapsedTime >= currentFrame.duration) {
            // Move to the next keyframe
            currentKeyFrame++;
            if (currentKeyFrame < numKeyFrames) {
                startTime = currentTime;
            }
        } else {
            // Move servos towards the target angles (you can add easing here)
            topEyelid.write(currentFrame.topEyelidAngle);
            bottomEyelid.write(currentFrame.bottomEyelidAngle);
            horizontal.write(currentFrame.horizontalAngle);
            vertical.write(currentFrame.verticalAngle);
        }

        return true; // Animation still running
    }
};

#endif //ANIMATION_H
