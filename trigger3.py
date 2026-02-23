import serial
import cv2
from ultralytics import YOLO
import time

print("✅ Classes: {0: 'head', 1: 'helmet'}")  # Your model confirmed

arduino = serial.Serial('COM7', 9600, timeout=1)
time.sleep(2)

model = YOLO('hemletYoloV8_100epochs.pt')

while True:
    if arduino.in_waiting > 0:
        try:
            line = arduino.readline().decode('latin-1').strip()
            print(f"📡 Arduino: '{line}'")
            
            if 'TRIGGER' in line:
                print("🎥 Camera ON")
                cap =cv2.VideoCapture(0)
                frames_checked = 0
                
                for i in range(30):
                    ret, img = cap.read()
                    if not ret: break
                    
                    helmet = False
                    
                    # **FIXED: conf=0.6, classes=[1] = HELMET ONLY**
                    results = model(img, conf=0.6, classes=[1])  # Class 1 = helmet
                    
                    if results[0].boxes is not None and len(results[0].boxes) > 0:
                        cls = int(results[0].boxes.cls[0])
                        conf = float(results[0].boxes.conf[0])
                        
                        print(f"Frame {i}: {model.names[cls]} Conf={conf:.2f}")
                        
                        # **CORRECT: Class 1 = helmet**
                        if cls == 1 and conf > 0.6:
                            helmet = True
                    
                    # Send to Arduino
                    if helmet:
                        arduino.write(b'H')
                        print("✓ SENT 'H' - HELMET DETECTED")
                    else:
                        arduino.write(b'N')
                        print("✗ SENT 'N' - NO HELMET")
                    
                    cv2.imshow('Helmet Check', img)
                    frames_checked += 1
                    
                    if cv2.waitKey(1) & 0xFF == ord('q'): break
                    if frames_checked >= 30: break
                
                cap.release()
                cv2.destroyAllWindows()
                
        except Exception as e:
            print(f"Ignored: {e}")
            continue
