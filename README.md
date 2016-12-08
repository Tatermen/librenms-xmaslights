# librenms-xmaslights

Twinkly Christmas lights synced to your LibreNMS alerts. This is quick and dirty, and there is no error handling. Use at your own risk.

## Hardware

You will need an Arduino Uno and FastLED compatible string of LED lights (eg. WS2811). Both can be purchased easily from sites like Amazon.

Connect the +5V and GND of the LEDs to the appropriate pins on the Arduino. Connect the data wire to pin 3 on the Arduino.

## Software

Upload the Arduino sketch (Arduino-XmasLights). Compile the C# console program - it will requre the Newtonsoft JSON library. 

## Running It

Run the LibreNMS-XmasLights.exe program. It will prompt you for the COM port of the Arduino, the LibreNMS URL and the LibreNMS API token. Note that the URL should be missing the final trailing slash. Eg:

```
Enter the COM Port to use: COM3
Enter the LibreNMS URL: http://librenms.example.com
Enter the LibreNMS API Token: i7aggh7438thg138

Serial Port Opened.
{
    "status": "ok",
    "err-msg": "",
    "count": 0,
    "alerts": []
}
```
