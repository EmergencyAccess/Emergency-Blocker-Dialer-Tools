# M911: Emergency Blocker/Dialer Tools

M911 Tools is a suite of applications designed to enhance the handling and interception of emergency calls in various network scenarios. The suite includes two main applications:

**Emerg-Call-Blocker**: Prevents 5G/4G/3G emergency calls from being delivered to Public Safety Answering Points (PSAPs). This tool intercepts and terminates emergency calls initiated by both cellular modems and IMS client applications, ensuring calls are blocked before they reach emergency services.

**Emerg-Call-Dialer**: Addresses issues faced by emergency users (UEs) in areas with poor signal reception. The app improves the reliability of emergency call connections by switching to the best available network, including Wi-Fi, and routing calls through local dispatch center phone numbers when necessary.

These tools provide crucial solutions for managing emergency communications, enhancing user safety, and ensuring efficient call handling in critical situations.

For more information, please refer to our publication.


## 1. Emerg-Call-Blocker
For comprehensive protection across all cellular generations—5G, 4G, and 3G—you can run Emerg-Call-Blocker-5G/4G alongside Emerg-Call-Blocker-3G. When an emergency call is initiated over a 5G or 4G network, Emerg-Call-Blocker-5G/4G detects and filters out the associated SIP messages. In scenarios where a weak cellular signal causes the phone to default to a 3G Circuit-Switched (CS) call, Emerg-Call-Blocker-3G will block these modem-initiated calls. Together, these tools provide a robust solution for preventing emergency calls from being routed to PSAPs during testing or other scenarios.
### Emerg-Call-Blocker-5G/4G
Emerg-Call-Blocker-5G/4G is designed to intercept and block emergency calls originating from IMS clients on 5G and 4G networks. The tool intercepts and drops SIP REGISTER/INVITE signaling at the IP layer, effectively preventing these calls from being routed to IMS servers and, ultimately, to PSAPs.

### Emerg-Call-Blocker-3G
This application is an extension of the Emerg-Call-Blocker-5G/4G, designed specifically for 3G networks.
In particular, emergency calls can originate from cellular modems (e.g., 3G CS calls) or IMS client applications (e.g., 4G/5G PS calls) on phones. For modem-initiated calls (e.g., 3G CS call), our tool leverages [Cellular Pro](https://play.google.com/store/apps/details?id=make.more.r2d2.google.cellular_pro&hl=en_US&pli=1), an application that collects cellular network signaling, to detect connection attempts specific to emergency calls and then terminates calls by monitoring signaling messages displayed on the screen via a customized accessibility service.

Emerg-Call-Blocker-3G is an extension of the Emerg-Call-Blocker-5G/4G tool, specifically designed for blocking emergency calls on 3G networks. Emergency calls on 3G networks typically originate from cellular modems using CS technology. To detect and block these calls, the tool leverages [Cellular Pro](https://play.google.com/store/apps/details?id=make.more.r2d2.google.cellular_pro&hl=en_US&pli=1), an application that collects cellular network signaling data. Emerg-Call-Blocker-3G monitors the signaling messages displayed on the phone screen through a customized accessibility service. The tool records the phone screen to capture each frame of Cellular Pro’s on-screen signaling, then processes these frames using the Python cv2 module and Optical Character Recognition (OCR) to detect specific keywords related to emergency calls.

Once a specific cellular signaling message, such as an RRC Connection Request, is detected, Emerg-Call-Blocker-3G terminates the call by executing a low-level shutdown of phone processes using the Android command `setprop sys.powerctl shutdown`. This ensures that 3G emergency calls are effectively blocked and prevented from reaching PSAPs.


---

### Tested Setup
* Computer: Dell XPS 13 with Intel i7-1195G7 CPU, 8GB RAM
* Operating System: Windows 11
* Python Version: Python 3.10
* Tested Mobile Devices (with Qualcomm Snapdragon CPU): Samsung Galaxy S21, Google
Pixel 5, LG G8X, and Motorola G Stylus 5G.

---
### Installation

#### Emerg-Call-Blocker-5G/4G
##### Prerequisites
Make sure you have the following installed on your system:
* Android Debug Bridge (ADB)
* ndk-build for Android (can be installed via Android Studio)

For **ndk-build**, ensure that your Windows/Linux system supports it. You may need to install Android Studio, which includes the NDK plugin necessary for building the project.

To install Android Studio and set up **ndk-build**:

1. Update and install required dependencies:

```
sudo apt update && sudo apt install git cmake ninja-build python python3-distutils openjdk-8-jdk qemu-kvm
```
2. Download Android Studio from https://developer.android.com/studio/ (ca. 1GB) and unpack the downloaded ZIP file. Open up a terminal and change to the directory to which you unpacked the ZIP file. To start Android Studio, run the following commands:
```
cd android-studio/bin
./studio.sh

# Follow the steps in the setup wizard to complete the installation.
```
**Note:** If you want to run Android Applications in the Android Emulator, follow the steps below. Reference: https://www.boden.io/getting-started/installing_dependencies/linux/

3. Install the NDK plugin:
```
Follow the steps in the reference to install NDK plugin.
Reference: https://developer.android.com/studio/projects/install-ndk
```

4. Locate your NDK path:

```
Example: /home/user/Android/Sdk/ndk/26.1.10909125/ndk-build
```

##### Folder Structure
```
Emerg-Call-Blocker/5G4G
|- jni (included external libs + build scripts) 
|- libs (generated folder when building)
|- obj (generated folder when building)
|- src (source code: We will only use main.c now.) 
```
##### Step 1: Clone the Repository
Clone the repository to your local machine using the following command:
```
git clone https://github.com/EmergencyAccess/Emergency-Blocker-Dialer-Tools.git
cd Emergency-Blocker-Dialer-Tools/Emerg-Call-Blocker/5G4G
```
##### Step 2: Build the Project
Run the following command in the source folder to build the project using ndk-build:
```
ndk-build
```
To clean the output:
```
ndk-build clean
```
##### Output
The default output is a tool similar to TCPdump that can be run directly on an Android device. By configuring the Android.mk file, you can also build a module that runs on the Android system.


---

#### Emerg-Call-Blocker-3G
##### Prerequisites
Make sure you have the following installed on your system:

* Python 3.7 or later
* pip (Python package installer)
* Android Debug Bridge (ADB)

##### Step 1: Clone the Repository
Clone the repository to your local machine using the following command:

```
git clone https://github.com/EmergencyAccess/Emergency-Blocker-Dialer-Tools.git
cd Emergency-Blocker-Dialer-Tools/Emerg-Call-Blocker/3G
```
##### Step 2: Install Dependencies
Install the required Python packages using pip. You can create a virtual environment to manage dependencies more easily:
1. Create a virtual environment (optional):
```
python3 -m venv venv
source venv/bin/activate  # On Windows use `venv\Scripts\activate`
```
2. Install the packages:
```
pip install adbnativeblitz
pip install opencv-python-headless
pip install numpy
pip install tesserocr
pip install Pillow
```
**Note**: AdbFastScreenshots offers a powerful and efficient way to capture Android device screens, making it a valuable tool for a wide range of applications and use cases. As fast as https://github.com/hansalemaos/adbblitz , but 100% native!


**Note**: The tesserocr package requires the Tesseract OCR engine. Make sure Tesseract is installed on your system. You can install it using the following instructions based on your operating system:
* **Windows**:
Download the Tesseract installer from the [official repository](https://github.com/tesseract-ocr/tesseract) and follow the installation instructions.

---
### Usage

#### Emerg-Call-Blocker-5G/4G
1. Push the Built File to Your Device
Once the project is built, you need to transfer the generated binary to your Android device. Use the following adb command to push the built file (usually located in the libs directory) to a directory on your device, such as /data/local/tmp:

    ```
    adb push libs/armeabi-v7a/block /data/local/tmp/
    ```
2. Change the file's permissions to make it executable:

    ```
    adb shell
    su
    chmod +x /data/local/tmp/block
    # Example: Block IPv6 ESP packets with a size over 900 bytes
    ./block -v 6 -p ESP -l 900  
    ```
3. Set up iptables to direct traffic to the NFQUEUE:
    * Insert the rule:
    ```
    ip6tables -I OUTPUT -j NFQUEUE --queue-num 0
    ```
    * Check the rule:
    ```
    ip6tables -L OUTPUT
    ```
4. Execute the `block` tool and make a 5G/4G phone call:
    * Get help:
    ```
    ./block -h
    ```
    * Block IPv4/v6 packets by protocol:
    ```
    # Protocol List: TCP, UDP, ESP
    ./block -v [IP_Version] -p [Protocol]
    ```
        
    * Block IPv4/v6 packets by packet length:
    ```
    ./block -v [IP_Version] -l [Length]
    
    # Example: To block IPv6 TCP packets with a size over 900 bytes (starting from the IP layer):
    ./block -v 6 -p TCP -l 900
    ```
    * Block **only** `SIP INVITE` messages over IPv4/IPv6 for TCP, UDP, and ESP using `-b` option:
    ```
    # Example: Block IPv4 SIP INVITE for TCP, UDP, and ESP on port 5060:
    ./block -v 4 -b -p 5060
    ```
5. Reset iptables after testing:
    * Remove the rule:
    ```
    ip6tables -D OUTPUT -j NFQUEUE --queue-num 0
    ```
    * Check the rule:
    ```
    ip6tables -L OUTPUT
    ```
    **Note**: Before making another call, please remember to set up the iptables rules again to ensure that traffic is properly directed to the block application
    
---
#### Emerg-Call-Blocker-3G
* Download [Split Screen Launcher](https://play.google.com/store/apps/details?id=com.ccssoftwareinc.splitscreenlauncher) from the Play Store for multi-tasking:
    1. Launch two apps on the screen simultaneously as the follow:
        (1) Cellular Pro
        (2) Phone app
        <img src="https://i.imgur.com/lr4xlv8.jpeg" alt="Launch two apps simultaneously" style="width: 50%;"/>
        **Note:** To improve the accuracy of text recognition, you can enlarge the CellularPro window.
    2. Filter out redundant messages in Cellular Pro:
        1. Click the `Filter` icon
        2. Deselect all options, then select only `UMTS-NAS` and `UMTS-RRC` from the menu 
        <img src="https://i.imgur.com/8Gi66wA.jpeg" alt="Cellular Pro filter" style="width: 50%;"/>

* Configure the Emerg-Call-Blocker-3G for real-time text detection:
    1. Update your ADB path:
    ```
    ADB_path = r"C:\Android\android-sdk\platform-tools\adb.exe"
    ```
    2. Confirm the desired area for text detection:
    ```
    # Add '-c' option to enable detection
    python3 emer-block-v4.py -s YOUR_DEVICE_ID -c

    # You can find your device ID by running the following command:
    adb devices
    # The device ID will be listed under the List of devices attached section.
    ```
        Select two points to define a rectangle for text detection. For example, the red top-left circle and the bottom-right circle in the following image will create a rectangular area:
    <img src="https://i.imgur.com/wzxMMDP.png" alt="Selected two points for text detection" style="width: 40%;"/>

    ![image](https://i.imgur.com/zkg9CpE.png)

    3. To use the Emerg-Call-Blocker-3G to detect 'RRC Connection Request' in your desired area, simply run:
    ```
    python3 emer-block-v4.py -s YOUR_DEVICE_ID -p P1_X P1_Y P2_X P2_Y

    # For example:
    python3 emer-block-v4.py -s ABCDEFGHIJ -p 397 430 1018 1563

    # You can use the '-d' option to display the monitored area:
    python3 emer-block-v4.py -s ABCDEFGHIJ -p 397 430 1018 1563 -d
    ```
    <img src="https://i.imgur.com/WQBpTcc.png" alt="Monitored area" style="width: 30%;"/>
    <img src="https://i.imgur.com/wLQgZ5C.png" alt="Monitored area" style="width: 30%;"/>

    Upon detecting the 'RRC Connection Request' message, the Emerg-Call-Blocker will send a low-level shutdown command to the device

**Evaluation**
For signal strengths ranging from good to poor in 5G/4G/3G networks, the signaling time from RRC Connection Request to 3G CC Setup or 5G/4G SIP REGISTER/INVITE is typically longer than 0.5 seconds and can extend up to 5 seconds when the signal strength is poor (we terminated testing when the time exceeded 5 seconds). However, the time required by Emerg-Call-Blocker from signaling message detection to call process shutdown is at most **0.4 seconds**, which is 20% shorter than the signaling time, ensuring prompt call termination before it is fully established.

**Note:** To further reduce the time cost, consider using a computer with a powerful CPU for the text detection procedure.

---
### Disclaimer
By using this software, you agree that you are doing so at your own risk. The authors and contributors of this project are not responsible for any damages, data loss, or other issues that may arise from the use of this software. It is provided "as is" without warranty of any kind, either express or implied. **You are solely responsible for ensuring that the software meets your needs and for any outcomes resulting from its use.**

**Important Notice:**
Sometimes the input screen source may be stuck and not be transmitted from the device to the computer running the Emerg-Call-Blocker. This may result in text detection not being performed in real time, further leading to the emergency call being made. Before dialing an emergency call, you must check if the text detection is working properly. We do not guarantee that the tool will work every time. It is recommended to test the tool by dialing a normal call first to ensure functionality. Additionally, you should be aware of each dial you make and terminate any unintended calls before they are forwarded to Public Safety Answering Points (PSAPs).
Alternatively, you may contact the author of [Cellular Pro](https://play.google.com/store/apps/details?id=make.more.r2d2.google.cellular_pro&hl=en_US&pli=1) to request a customized version for an additional cost. This version would include Android logcats indicating each RRC message, helping to better determine emergency dials promptly.

---
### Credits
We sincerely appreciate the [Cellular Pro](https://play.google.com/store/apps/details?id=make.more.r2d2.google.cellular_pro&hl=en_US&pli=1) and [AdbFastScreenshots](https://github.com/hansalemaos/adbnativeblitz) teams for making their great softwares available.


## 2. Emerg-Call-Dialer
We developed the “Emerg-Call-Dialer” mobile app to address issues faced by emergency users (UEs) in RANs with poor signals when initiating emergency calls. The app handles call requests from emergency UEs in two scenarios: (1) If signals from home PLMN’s observed 3GPP RANs are weak while those from visited PLMNs are moderate or good, the app temporarily disables the SIM/eSIM, switching the UE to an anonymous mode to access available nearby RANs; and (2) If signals from both home and visited PLMNs are weak but Wi-Fi RANs with moderate or good signals are available, the app initiates emergency calls through VoWiFi by translating the dialed emergency number to the local dispatch center’s phone number (e.g., 248-796-5500 for the City of Southfield in the U.S.) and placing the call using telephony APIs like TelecomManager.placeCall() on Android. Users need to configure their UEs to prefer Wi-Fi calling for this scenario.

---
### Tested Setup
* Tested Mobile Device (with Qualcomm Snapdragon CPU): Samsung Galaxy S21, Google
Pixel 5
* Android Version: 11

---
### Installation

#### Prerequisites
Make sure you have the following installed on your system:

* Android Studio installed on your computer.
* An Android device or emulator running Android 5.0 (Lollipop) or higher.
* Ensure that ADB (Android Debug Bridge) is set up and the device is connected.

#### Step 1: Clone the Repository
Clone the repository to your local machine using the following command:

```
git clone https://github.com/EmergencyAccess/Emergency-Blocker-Dialer-Tools.git
cd Emergency-Blocker-Dialer-Tools/Emergency-Dialer
```
#### Step 2: Open the Project in Android Studio
1. Open Android Studio:

    * Launch Android Studio on your computer.
1. Import the Project:

    * Click on `File` -> `Open...`.
    * Navigate to the directory where you cloned the repository and select the project folder. Click `OK` to open the project.
1. Wait for Gradle Build:

    * Android Studio will automatically start the Gradle build process. This may take a few minutes depending on your system and internet speed. Make sure all dependencies are resolved.
#### Step 3: Configure the Project
1. Set Up Your Device:

    * Connect your Android device to your computer via USB.
    * Enable `Developer Options` and `USB Debugging`  on your device.

#### Step 4: Add your local PSAP information
1. Update the county_numbers.json File
    * Locate the county_numbers.json file in the project directory:
    ``` 
    /app/src/main/assets/county_numbers.json.
    ```
    * Open the county_numbers.json file in a text editor.
    * Add your local PSAP (Public Safety Answering Point) information to the JSON file. The file stores pairs of County Name and its local PSAP number. Ensure the information is correctly formatted, for example:
    ```
    {
        "county_numbers": [
            {
                "county": "Your County Name",
                "number": "Your PSAP Number"
            },
            {
                "county": "Another County Name",
                "number": "Another PSAP Number"
            }
        ]
    }
    ```
    **Note:** We have preloaded the local PSAP information for Michigan. Make sure to add and save your local PSAP information in the county_numbers.json file before proceeding.

#### Step 5: Run the Application
1. Run on Device:

    * Click on the `Run` button in Android Studio or press `Shift + F10`.
    * Select your connected device or emulator from the list and click `OK`.
    * The application will be built and installed on your device.
1. Configure Wi-Fi Calling:

    * Ensure that your device is configured to prefer Wi-Fi calling. This can typically be done in the phone's `settings` under `Network & Internet` -> `Mobile network` -> `Advanced` -> `Wi-Fi calling`.

**Note**: Make sure your Android device has the necessary permissions to access phone features and network settings required by the Emerg-Call-Dialer app

---
### Usage
Use the Emerg-Call-Dialer as your phone application for initiating emergency calls. The following image shows the dialing panel of the Emerg-Call-Dialer.

<img src="https://i.imgur.com/RY6fQWh.jpeg" alt="Emerg-Call-Dialer Screenshot" style="width: 30%;"/>

To make an emergency call, open the Emerg-Call-Dialer app and use the dialing panel just as you would with any standard phone application. The app will automatically handle the call request based on the network conditions, ensuring that the call is routed through the most reliable network available.

**Optional:** In the app, there is a pre-defined cellular signal strength threshold set at -120 dBm. If the signal from the home cell is weaker than this threshold while signals from visited PLMNs (Public Land Mobile Networks) are moderate or good, the app will temporarily disable the SIM/eSIM, switching the User Equipment (UE) to an anonymous mode to access available nearby Radio Access Networks (RANs). Users can configure this threshold value in the app by simply typing the desired value into the threshold field and then clicking the "save" button. From that point onward, the customized threshold will be applied.

**Note:** To prevent unintended emergency calls, you can modify the app to replace the emergency number with a normal number before the dial intent is broadcast. Ensure that you take full responsibility for any dialed emergency calls.

---
### Disclaimer
By using this software, you agree that you are doing so at your own risk. The authors and contributors of this project are not responsible for any damages, data loss, or other issues that may arise from the use of this software. It is provided "as is" without warranty of any kind, either express or implied. **You are solely responsible for ensuring that the software meets your needs and for any outcomes resulting from its use.**

**Important Notice:**
The Emerg-Call-Dialer app is only validated on the tested setup. You should be aware of each dial you make and terminate any unintended calls before they are forwarded to Public Safety Answering Points (PSAPs).

