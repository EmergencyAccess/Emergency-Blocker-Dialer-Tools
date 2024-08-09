from adbnativeblitz import AdbFastScreenshots
import time
import cv2
import numpy as np
import tesserocr
import subprocess
import argparse
from PIL import Image, ImageEnhance, ImageFilter
import threading
import sys

ADB_path = r"C:\Android\android-sdk\platform-tools\adb.exe"

lock = threading.Lock()

def preprocess_image(image):
    """
    Preprocess the image to improve OCR accuracy.

    :param image: Image object
    :return: Preprocessed Image object
    """
    # Convert image to grayscale
    img = image.convert('L')
    
    # Apply a slight blur to reduce noise
    img = img.filter(ImageFilter.GaussianBlur(radius=1))
    
    # Enhance the image contrast
    enhancer = ImageEnhance.Contrast(img)
    img = enhancer.enhance(2)
    
    # Apply a threshold to get a binary image
    img = img.point(lambda x: 0 if x < 128 else 255, '1')
    
    return img


def detect_text(image):
    """
    Detect text in an image using Tesseract OCR with preprocessing.

    :param image: Image object
    :return: Detected text as a string
    """
    # Preprocess the image
    preprocessed_image = preprocess_image(image)
    
    # Use tesserocr to do OCR on the image
    text = tesserocr.image_to_text(preprocessed_image)
    
    return text

def add_logcat_record(device_id=None, msg=""):
    try:
        # adb command to be executed
        command = ["adb", "shell", "log", "-t", "M911", msg]

        # Add the device ID if provided
        if device_id:
            command = ["adb", "-s", device_id, "shell", "log", "-t", "M911", msg]

        # Execute the adb logcat command
        result = subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            
        # Print the result of the command
        print(result.stdout.decode())
    except subprocess.CalledProcessError as e:
        # Handle errors in case the command fails
        print(f"Error: {e.stderr.decode()}")





def threaded_detection(device_id, cropped_image, start_time):
    if lock.acquire(blocking=False):
        try:
            start_detect_text(device_id, cropped_image)
            # Record the end time
            end_time_detection = time.time()
            elapsed_time = end_time_detection - start_time
            print(f"Time elapsed: {elapsed_time:.4f} seconds")
        finally:
            lock.release()
    # else:
        

def start_detect_text(device_id, screenshot):
    
    # Add logcat record
    thread = threading.Thread(target=add_logcat_record, args=(device_id, "Start text detection!"))
    thread.start()

    # Convert the screenshot to a numpy array
    screenshot = Image.fromarray(cv2.cvtColor(screenshot, cv2.COLOR_RGB2BGR))
    
    # Detect text in the screenshot
    text = detect_text(screenshot)
    
    # Add timestamp to logcat
    thread = threading.Thread(target=add_logcat_record, args=(device_id, "End text detection!"))
    thread.start()

    # Print the detected text   
    print(f"Detected text: {text}")

    # Please modify your desired text
    if ("Complete" in text):
        print("'RRC Connection Setup Complete' detected!")
        # adb_airplane_mode(device_id)
        adb_shutdown(device_id)
        thread = threading.Thread(target=add_logcat_record, args=(device_id, "Send Shutdown message!"))
        thread.start()
        # add_logcat_record(device_id, "Send Shutdown message!")

    
    # cv2.destroyAllWindows()

def adb_airplane_mode(device_id=None):
    """
    Turn on Airplan Mode using adb.
    """
    try:
        # Define the base command
        command = ["adb", "shell", "cmd", "connectivity airplane-mode enable"]

        # Add the device ID if provided
        if device_id:
            command = ["adb", "-s", device_id, "shell", "cmd", "connectivity airplane-mode enable"]

        # Execute the adb reboot command
        result = subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        # Print the result of the command
        print(result.stdout.decode())
    except subprocess.CalledProcessError as e:
        # Handle errors in case the command fails
        print(f"Error: {e.stderr.decode()}")

def adb_shutdown(device_id=None):
    """
    Excecute low-level and instant shutdown command using adb.
     
    """
    try:
        # Define the base command
        command = ["adb", "shell", "su", "-c", "setprop sys.powerctl reboot,recovery"]

        # Add the device ID if provided
        if device_id:
            command = ["adb", "-s", device_id, "shell", "su", "-c", "setprop sys.powerctl reboot,recovery"]

        # Execute the adb reboot command
        result = subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        # Print the result of the command
        print(result.stdout.decode())
        
    except subprocess.CalledProcessError as e:
        # Handle errors in case the command fails
        print(f"Error: {e.stderr.decode()}")

def get_coordinates(event, x, y, flags, param):
    """
    Obtain the coordination of where you click your cursor.
     
    """
    if event == cv2.EVENT_LBUTTONDOWN:
        print(f"Left click at position: ({x}, {y})")
    elif event == cv2.EVENT_RBUTTONDOWN:
        print(f"Right click at position: ({x}, {y})")

if __name__ == "__main__":

    # Initialize the parser
    parser = argparse.ArgumentParser(description="Select Android device")
    
    # Add the device ID argument
    parser.add_argument("-s", "--device", help="The ID of the device")
    # Option for detecting -c
    parser.add_argument("-c", action="store_true", help="Enable the function to detect the coordination of your cursor")
    # Option for displaying the detected area
    parser.add_argument("-d", action="store_true", help="Enable the function to show the area for text detection") 
    # Option for providing coordinates
    parser.add_argument('-p', type=int, nargs=4, metavar=('x1', 'y1', 'x2', 'y2'),
                        help="Provide coordinates for two points as x1 y1 x2 y2")
    
    # Parse the arguments
    args = parser.parse_args()
    if args.c:
        print("Begin detection: Click to the left or right of your cursor to display the coordinates, and press 'q' to exit.")
    if args.p:
        if len(args.p) != 4:
            print("Error: You must provide exactly four coordinates (x1, y1, x2, y2).")
            sys.exit(1)
        x1, y1, x2, y2 = args.p
        print(f"Coordinates received: ({x1}, {y1}) and ({x2}, {y2})")
    
    # Call the function with the provided device ID
    device_id = args.device
    
    # Sample target region:
    # Point 1: The top-left corner of the selected area
    # x1_coor = 837
    # y1_coor = 384

    # Point 2: The bottom-right corner of the selected area
    # x2_coor = 1068
    # y2_coor = 1477

    
    with AdbFastScreenshots(
		adb_path=ADB_path,
        device_serial= device_id,
		time_interval=1,
		width=1080,
		height=2400,
		bitrate="2M",
		use_busybox=False,
		connect_to_device=False,
		screenshotbuffer=100,
		go_idle=0,
    ) as adbscreen:
        for image in adbscreen:
            # Record the start time
            start_time = time.time()

            if args.c:
                    # Enable the function of get_coordinates
                    cv2.namedWindow('Image')
                    cv2.setMouseCallback('Image', get_coordinates)
                    # Show the entire screen for users to determine coordinations
                    cv2.imshow("Image", image)
            else:
                cropped_image = image[y1:y2, x1:x2+100]
                thread = threading.Thread(target=threaded_detection, args=(device_id, cropped_image, start_time))
                thread.start()
                if args.d:
                    # Show the monitored area
                    cv2.imshow("Image", cropped_image)

            if cv2.waitKey(1) & 0xFF == ord("q"):
                break
        cv2.destroyAllWindows()



