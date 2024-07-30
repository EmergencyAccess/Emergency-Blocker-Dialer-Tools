# M911 Tools

M911 Tools is a suite of applications designed to enhance the handling and interception of emergency calls in various network scenarios. The suite includes two main applications:

**Emerg-Call-Blocker**: Prevents 5G/4G/3G emergency calls from being delivered to Public Safety Answering Points (PSAPs). This tool intercepts and terminates emergency calls initiated by both cellular modems and IMS client applications, ensuring calls are blocked before they reach emergency services.

**Emergency Dialer**: Addresses issues faced by emergency users (UEs) in areas with poor signal reception. The app improves the reliability of emergency call connections by switching to the best available network, including Wi-Fi, and routing calls through local dispatch center phone numbers when necessary.

These tools provide crucial solutions for managing emergency communications, enhancing user safety, and ensuring efficient call handling in critical situations.

For more information, please refer to our publication.


## Emerg-Call-Blocker

This application can prevent 5G/4G/3G emergency calls from being delivered to PSAPs. In particular, emergency calls can originate from cellular modems (e.g., 3G CS calls) or IMS client applications (e.g., 4G/5G PS calls) on phones. If initiated by the IMS client app, Emerg-Call-Blocker intercepts and drops all call signaling (e.g., SIP REGISTER/INVITE). For modem-initiated calls (e.g., 3G CS call), our tool leverages Cellular Pro [28], an application that collects cellular network signaling, to detect connection attempts specific to emergency calls and then terminates calls by monitoring signaling messages displayed on the screen via a customized accessibility service.


---

### Tested Setup
* Computer: Dell XPS 13 with Intel i7-1195G7 CPU, 8GB RAM
* Operating System: Windows 11
* Python Version: Python 3.10
* Tested Mobile Device: Samsung S21 (with Qualcomm Snapdragon CPU)

---
### Installation

#### Prerequisites
Make sure you have the following installed on your system:

* Python 3.7 or later
* pip (Python package installer)
* Android Debug Bridge (ADB)

#### Step 1: Clone the Repository
Clone the repository to your local machine using the following command:

```
git clone https://github.com/kyle0121chen/M911.git
cd M911/Emerg-Call-Blocker
```
#### Step 2: Install Dependencies
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

To use the script, simply run:
```
python3 emer-block-v4.py -s YOUR_DEVICE_ID
```
You can find your device ID by running the following command:
```
adb devices
```
The device ID will be listed under the List of devices attached section.


---
### Credits
We sincerely appreciate the [Cellular Pro](https://play.google.com/store/apps/details?id=make.more.r2d2.google.cellular_pro&hl=en_US&pli=1) and [AdbFastScreenshots](https://github.com/hansalemaos/adbnativeblitz) teams for making their great softwares available.

---
### Disclaimer
By using this software, you agree that you are doing so at your own risk. The authors and contributors of this project are not responsible for any damages, data loss, or other issues that may arise from the use of this software. It is provided "as is" without warranty of any kind, either express or implied. **You are solely responsible for ensuring that the software meets your needs and for any outcomes resulting from its use.**

**Important Notice:**
Sometimes the input screen source may be stuck and not be transmitted from the device to the computer running the Emerg-Call-Blocker. This may result in text detection not being performed in real time. Before dialing an emergency call, you must check if the text detection is working properly. It is recommended to test the tool by dialing a normal call first to ensure functionality. Additionally, you should be aware of each dial you make and terminate any unintended calls before they are forwarded to Public Safety Answering Points (PSAPs).






## Emergency Dialer
We developed the “Emergency Dialer” mobile app to address issues faced by emergency users (UEs) in RANs with poor signals when initiating emergency calls. The app handles call requests from emergency UEs in two scenarios: (1) If signals from home PLMN’s observed 3GPP RANs are weak while those from visited PLMNs are moderate or good, the app temporarily disables the SIM/eSIM, switching the UE to an anonymous mode to access available nearby RANs; and (2) If signals from both home and visited PLMNs are weak but Wi-Fi RANs with moderate or good signals are available, the app initiates emergency calls through VoWiFi by translating the dialed emergency number to the local dispatch center’s phone number (e.g., 248-796-5500 for the City of Southfield in the U.S.) and placing the call using telephony APIs like TelecomManager.placeCall() on Android. Users need to configure their UEs to prefer Wi-Fi calling for this scenario.

---

### Tested Setup
* Tested Mobile Device: Samsung S21 (with Qualcomm Snapdragon CPU)
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
git clone https://github.com/kyle0121chen/M911.git
cd M911/Emergency-Dialer
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

#### Step 4: Run the Application
1. Run on Device:

    * Click on the `Run` button in Android Studio or press `Shift + F10`.
    * Select your connected device or emulator from the list and click `OK`.
    * The application will be built and installed on your device.
1. Configure Wi-Fi Calling:

    * Ensure that your device is configured to prefer Wi-Fi calling. This can typically be done in the phone's `settings` under `Network & Internet` -> `Mobile network` -> `Advanced` -> `Wi-Fi calling`.

**Note**: Make sure your Android device has the necessary permissions to access phone features and network settings required by the Emergency Dialer app



---



### Usage
Use the Emergency Dialer as your phone application for initiating emergency calls. The following image shows the dialing panel of the Emergency Dialer.

<img src="https://i.imgur.com/N6OK7wj.jpeg" alt="Emergency Dialer Screenshot" style="width: 30%;"/>


To make an emergency call, open the Emergency Dialer app and use the dialing panel just as you would with any standard phone application. The app will automatically handle the call request based on the network conditions, ensuring that the call is routed through the most reliable network available.

**Note:** To prevent unintended emergency calls, you can modify the app to replace the emergency number with a normal number before the dial intent is broadcast. Ensure that you take full responsibility for any dialed emergency calls.


---
### Disclaimer
By using this software, you agree that you are doing so at your own risk. The authors and contributors of this project are not responsible for any damages, data loss, or other issues that may arise from the use of this software. It is provided "as is" without warranty of any kind, either express or implied. **You are solely responsible for ensuring that the software meets your needs and for any outcomes resulting from its use.**

**Important Notice:**
The Emergency Dialer app is only validated on the tested setup. You should be aware of each dial you make and terminate any unintended calls before they are forwarded to Public Safety Answering Points (PSAPs).

