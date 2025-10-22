import cv2
import serial
import time

# Initialize camera
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1000)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 600)

# Connect to Arduino
arduino = serial.Serial('COM16', 9600, timeout=1)
time.sleep(2)

# Load Haar Cascade for face detection
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

# Servo center positions (for when no face is detected)
center_x = 500
center_y = 300

while True:
    ret, frame = cap.read()
    if not ret:
        continue

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.2, 5)

    if len(faces) > 0:
        # Take the largest face
        x, y, w, h = max(faces, key=lambda f: f[2] * f[3])
        cx = x + w // 2
        cy = y + h // 2

        # Draw rectangle
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        cv2.circle(frame, (cx, cy), 5, (255, 0, 0), -1)

        # Send to Arduino
        arduino.write(f"{cx},{cy}\n".encode())
        last_seen = time.time()
    else:
        # If no face detected for 2 seconds, look straight
        if time.time() - locals().get("last_seen", 0) > 2:
            arduino.write(f"{center_x},{center_y}\n".encode())

    cv2.imshow("Face Tracking", frame)

    if cv2.waitKey(5) & 0xFF == 27:
        break

arduino.close()
cap.release()
cv2.destroyAllWindows()
