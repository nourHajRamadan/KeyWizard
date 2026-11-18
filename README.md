# KeyWizard

A HID built for Waveshare ESP32S3 3.5 Devboard with many features

Key Functionality:
- HID for any OS (Mouse, Keyboard, Media Control)
- NVS based encrypted password and favourites manager
- Dedicated CLI over Serial USB for managing NVS content
- SD card dependent script and clipboard typing
- MP3 Player (live streams over wifi)
- Wifi Manager custom (Remote HID Interface, Wifi config, OTA,..)

In Detail description:

#Main Screen
<img width="480" height="320" alt="Screen1" src="https://github.com/user-attachments/assets/339e7fcd-6e50-4598-8b9d-09f514d97e13" />
- With the slider underneath, all features can be shown and then respectivly clicked on.
- The OTA button activates the WifiManager Portal with many features such as Remote Interface and OTA, it requires the 6-PIN code in the Passkey window.
- The Power Off button turns the board off using the internal PMU. It can be turned on again with the power button on the board or by connecting USB-C.
- The percentage refers to the level of the attached battery or says "USB-C" without one.
- A label on the left upper side appears in case of Wifi connectivity with alternating clock+date and ip address.
- The KeyWizard logo is also a button that activates config mode to enter the CLI via Serial Monitor (Baudrate 115200).

#Mouse
<img width="480" height="320" alt="Screen10" src="https://github.com/user-attachments/assets/f1870fed-1923-4ec3-a13e-3eef5540471e" />
- Space as mousepad.
- Buttons on either side for respective mouse click.
- Middle button for repeated left-click. Activates once pressed for a while and deactivates on first touch.

#Keyboard
<img width="480" height="320" alt="Screen2" src="https://github.com/user-attachments/assets/f5e93c26-8f25-4df0-896b-f5f0f4c79f46" />
- Usual LVGL keyboard with custom font to fit board.
- ENTER send text to the host.
- Plus button adds content of box to clipboard (saved on SD card).
- Right arrow leads to the clipboard window (up next).
- Shortcuts symbol to go to Shortcuts window.

#Clipboard
<img width="480" height="320" alt="Screen8" src="https://github.com/user-attachments/assets/cfd201e6-ae5e-437a-82ef-5c988d04f5f6" />
- Scroll through Clipboard elements.
- Print button prints element to host.
- Enter button triggers enter key on host.
- Del button deletes selected element.
- WoL button uses content of element as a MAC address to send a magic packet over local network.
- Shortcuts symbol to go to Shortcuts window.

#Shortcuts
<img width="480" height="320" alt="Screen3" src="https://github.com/user-attachments/assets/8803b924-3a5a-4fc1-af09-8232b2ceaed0" />
- Many known keyboard shortcuts to trigger respectivly on host.
- Some of them support repeated triggering while pressing.
- Menus for Win, Ctrl combis as well as Fx buttons.
- Some media buttons that usually work on most systems.

#Passkeys (Password Manager)
<img width="480" height="320" alt="Screen5" src="https://github.com/user-attachments/assets/415bb067-aa1a-4b3c-aeea-d01ba0cf9ac5" />
- Allows entering a configured 6-digit-pin (via CLI).
- Scroll to choose saved encrypted password strings (element in NVS consists of KEY and STRING/VALUE, KEY is visible in list to choose from, VALUE could be username\tpassword).
- Pressing the Enter button triggers the decryption, which takes (for one time) about 20-30 seconds, and then prints VALUE to host.

#Scripts
<img width="480" height="320" alt="Screen6" src="https://github.com/user-attachments/assets/71f260c0-6e67-4e89-bac3-457bfaf7b2ee" />
- Scroll through filenames of txt files saved on SD card under /scripts.
- Enter button prints content of txt file (without considerate pauses, line after line) after triggering cmd via Win+R.
- Enabling powershell checkbox replaces cmd as an argument in Win+R with powershell.

#Favourites
<img width="480" height="320" alt="Screen7" src="https://github.com/user-attachments/assets/1d66baad-fa88-4060-93cd-b093cfa63130" />
- Favourites are also dependent on the 6-digit-pin entered in passkeys and require decryption.
- They work in the same princible as passkeys (also configurable via CLI).
- Enter prints VALUE of selected element.
- Checking Win+R makes the Enter button print VALUE into Win+R box which is helpful in opening links in system browser for example.

#MP3 Player
<img width="480" height="320" alt="Screen4" src="https://github.com/user-attachments/assets/7e061a57-b7a7-4938-b1fe-8f6d922102a4" />
- Scrolling through mp3 files from /music on SD card.
- At the begin of the list appear the elements of the urls.txt file in /music (for http streams, format: NAME|LINK\n, requires Wifi connectivity).
- Usual Control buttons. Rapid clicking might crash the board.
- Info of track or stream shown as much as possible.
- Automatic shuffle after track ends.
- Indicator is able of seeking but not very flexible or reliable.

#Black Screen
<img width="480" height="320" alt="Screen9" src="https://github.com/user-attachments/assets/f6bfd5b9-5326-4ae6-8417-8c699a1853c1" />
- After a timeout, the screen goes black to save the screen and power.
- If shook lightly or touched, the screen lights up again, but shows this screen first to avoid accidental touching.
- Slider unlocks to Main Screen.
- MP3 Player info shown at the buttom if active.
- Clock is shown in the left lower corner if Wifi is active.
- Battery percentage is shown in the right lower corner.
- Clicking on the KeyWizard Symbol tells the time via audio in English.
- If entered through KeyWizard symbol on Main Screen, this screen isn't escapable without reboot.

More details and info to follow...
Use the release files to flash the complete system (BIN file) to Waveshare 3.5inch Devboard.
(functional without SD card, SD card should be in FAT32)
