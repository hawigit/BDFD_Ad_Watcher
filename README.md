# BDFD Ad watcher
There's an app, an organization, a language, called BDFD, aka, Bot Designer For Discord. Their mobile app lets us host discord bots for free in exchange for watching ads.
So I made this macro that watches the ad for me 😀.

![Main Application screenshot](Readme_asset/Mainuse.png)

### Requirements
* Runs on **Windows**.
* Uses **Bluestacks** emulator (running the BDFD app downloaded from Google Play Store).

## Installation
1. Go to [BDFD Ad Watcher v5.0](https://github.com/phivogit/BDFD_Ad_Watcher/releases/latest) and download the `.exe` file.
2. Run the file, follow the setup wizard to complete setting up the app.
3. In Bluestacks, go to **Settings** > **Advanced** and enable **Android Debug Bridge (ADB)** to allow connecting to Android.

![Bluestacks ADB Settings](Readme_asset/BluestacksConfig.png)

## Usage
1. Open Bluestacks, open BDFD app, click on your bot.
3. Make sure you don't scroll down, go to BDFD Ad Watcher app, click **Connect to bluestacks**, then click **Start**.
4. To stop the program, click **Stop**.

## How to Configure Settings
- Button locations: pretty self-explainatory, it sets the button locations so the app knows where to click in each state.
- Templates: the app uses pictures of elements (called template) to decide the current state. For example, if it sees the **Ad watch button** (based on the Ad watch button template), the state will be 0, it will then click on the ad watch button. 

![Configuration Tab Screenshot](Readme_asset/Config_Tab.png)

### Setting Coordinates & Templates
* **Set Location Buttons (1 Click)**:
  Used to set the coordinates for clicking buttons (e.g. X Close button, Ad Watch button). Make a **single click** on the preview to set the button location.
* **Template Saving Buttons (2 Clicks)**:
  Used to capture image templates of buttons and elements to detect states. Make **two clicks** on the preview to save the template:
  1. First click: **Top-Left** corner of the element.
  2. Second click: **Bottom-Right** corner of the element.

### File & Settings Locations
* **Saved Settings File**: `C:\Users\<Username>\AppData\Local\appBDFDAdW\settings.json`
* **Templates Folder**: `C:\Users\<Username>\AppData\Local\appBDFDAdW\templates\`
* **Screenshots (temp files)**: `C:\Users\<Username>\AppData\Local\appBDFDAdW\` (`screenshot.png`, `temp_captcha.png`)