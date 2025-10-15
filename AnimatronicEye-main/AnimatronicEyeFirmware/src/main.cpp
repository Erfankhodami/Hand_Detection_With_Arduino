#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand()

#include <ESP32Servo.h>

#ifndef USE_JOYSTICK_INPUT
#include "KeyFrame.h"
#include "Animation.h"
#endif

// Create Servo objects
Servo topEyelid, bottomEyelid, horizontal, vertical;

// Define min and max angles for each servo
int minTopEyelid = 90, maxTopEyelid = 20;
int minBottomEyelid = 90, maxBottomEyelid = 180;
int minHorizontal = 60, maxHorizontal = 130;
int minVertical = 60, maxVertical = 120;

// Pins for servos
int pinTopEyelid = 25;
int pinBottomEyelid = 33;
int pinHorizontal = 32;
int pinVertical = 26;

// Pins for joystick
int pinX = 14;
int pinY = 12;
int pinSW = 13;

// Macro for build-time configuration
#ifdef USE_JOYSTICK_INPUT
int servoAngle(const int analogInput, const int minAngle, const int maxAngle) {
    // stop jittering when joystick is centered
    if (analogInput > 2500 && analogInput < 3400) {
        return (minAngle + maxAngle) / 2;
    }

    return map(analogInput, 0, 4095, minAngle, maxAngle);
}
#else

KeyFrame smartKeyFrame(const int horizontalAngle, const int verticalAngle, const unsigned long duration) {
    const int maxTopEyelid2 = maxTopEyelid-(0.10*maxTopEyelid);
    const int minTopEyelid2 = minTopEyelid+(0.05*minTopEyelid);
    const int maxBottomEyelid2 = maxBottomEyelid+(0.10*maxBottomEyelid);
    const int minBottomEyelid2 = minBottomEyelid-(0.05*minBottomEyelid);
    const int topEyelidAngle = map(verticalAngle, minVertical, maxVertical, maxTopEyelid2, minTopEyelid2);
    const int bottomEyelidAngle = map(verticalAngle, minVertical, maxVertical, minBottomEyelid2, maxBottomEyelid2);
    return KeyFrame(topEyelidAngle, bottomEyelidAngle, horizontalAngle, verticalAngle, duration);
}

// Animations
KeyFrame blinkAnimationFrames[] = {
    KeyFrame(20, 180, 90, 90, 500),  // Open eyelids over 500ms
    KeyFrame(90, 90, 90, 90, 150)    // Close eyelids over 500ms
};

KeyFrame sideToSideBlinkAnimationFrames[] = {
    KeyFrame(40, 160, minHorizontal, 90, 700),  // min
    KeyFrame(90, 90, 100, 90, 150),
    KeyFrame(40, 160, maxHorizontal, 90, 700), // max
    KeyFrame(90, 90, 100, 90, 150),
};

KeyFrame diagonalSideToSideBlinkAnimationFrames[] = {
    smartKeyFrame(minHorizontal, minVertical, 500),  // min
    smartKeyFrame(maxHorizontal, minVertical, 500),  // min
    KeyFrame(90, 90, 100, 90, 150),
    smartKeyFrame(minHorizontal, maxVertical, 500),  // max
    smartKeyFrame(maxHorizontal, maxVertical, 500),  // max
    KeyFrame(90, 90, 100, 90, 150),
};

KeyFrame rollingEyesFrames[] = {
    smartKeyFrame(90, 60, 75),   // Up
    smartKeyFrame(130, 60, 75),  // Top-right
    smartKeyFrame(130, 90, 75),  // Right
    smartKeyFrame(130, 120, 75), // Bottom-right
    smartKeyFrame(90, 120, 75),  // Down
    smartKeyFrame(60, 120, 75),  // Bottom-left
    smartKeyFrame(60, 90, 75),   // Left
    smartKeyFrame(60, 60, 75),   // Top-left
    smartKeyFrame(90, 60, 75),   // Back to Up
};

KeyFrame centeredEyeAnimationFrames[] = {
    smartKeyFrame((minHorizontal + maxHorizontal) / 2, (minVertical + maxVertical) / 2, 1000),
};

// Create Animation objects from the keyframes
Animation blinkAnimation(blinkAnimationFrames, 2);
Animation sideToSideAnimation(sideToSideBlinkAnimationFrames, 4);
Animation diagonalSideToSideAnimation(diagonalSideToSideBlinkAnimationFrames, 6);
Animation centeredEyeAnimation(centeredEyeAnimationFrames, 1);
Animation rollingEyesAnimation(rollingEyesFrames, sizeof(rollingEyesFrames) / sizeof(KeyFrame));

// Store the animations in an array for easy access
Animation *animations[] = {
    &blinkAnimation,
    &sideToSideAnimation,
    &diagonalSideToSideAnimation,
    &centeredEyeAnimation,
    &rollingEyesAnimation,
};
int currentAnimationIndex = 0;
Animation *activeAnimation = animations[currentAnimationIndex];

// Function to switch to a random animation
void switchToRandomAnimation() {
    int randomIndex = rand() % 5; // Generate random index between 0 and 3
    activeAnimation = animations[randomIndex];
    activeAnimation->start(); // Start the new animation
}

#endif

void shuteye() {
    topEyelid.write(minTopEyelid);
    bottomEyelid.write(minBottomEyelid);
}

void openeye() {
    topEyelid.write(maxTopEyelid);
    bottomEyelid.write(maxBottomEyelid);
}

void setup() {
    Serial.begin(115200);
    pinMode(pinSW, INPUT_PULLUP);

    // Attach servos to their respective pins
    topEyelid.attach(pinTopEyelid);
    bottomEyelid.attach(pinBottomEyelid);
    horizontal.attach(pinHorizontal);
    vertical.attach(pinVertical);

    // Start all servos in the center position
    topEyelid.write(maxTopEyelid);
    bottomEyelid.write(maxBottomEyelid);
    horizontal.write(90);
    vertical.write(90);

    // Seed random number generator
    srand(esp_random());
}

void loop() {
    #ifdef USE_JOYSTICK_INPUT
    const int x = analogRead(pinX);
    const int y = analogRead(pinY);
    const int pressed = digitalRead(pinSW);
    Serial.printf("X: %d, Y: %d, SW: %d\n", x, y, pressed);
    delay(20);

    const int horizontalAngle = servoAngle(x, minHorizontal, maxHorizontal);
    const int verticalAngle = servoAngle(y, minVertical, maxVertical);
    horizontal.write(horizontalAngle);
    vertical.write(verticalAngle);

    if (!pressed) {
        shuteye();
    } else {
        // adjust eyelids according to vertical angle
        const int topEyelidAngle = map(verticalAngle, minVertical, maxVertical, maxTopEyelid, minTopEyelid);
        const int bottomEyelidAngle = map(verticalAngle, minVertical, maxVertical, minBottomEyelid, maxBottomEyelid);
        topEyelid.write(topEyelidAngle);
        bottomEyelid.write(bottomEyelidAngle);
    }

    #else
    if (!activeAnimation->update(topEyelid, bottomEyelid, horizontal, vertical)) {
        switchToRandomAnimation(); // Switch to another animation when the current one ends
    }
    #endif
}
