#include "bluetooth_dashboard.h"

BluetoothSerial SerialBT;

extern uint8_t channels_resolution[];

#define maxGPIOPins 49

// Global variables to capture PMW pins
const int maxChannels = 64;
int ledcChannelPin[maxChannels][2];
int ledcChannelPinCount = 0;
int ledcChannelResolution[maxChannels][2];
int ledcChannelResolutionCount = 0;


// Macro to trap values pass to ledcAttachPin since there is no ESP32 API
#define ledcAttachPin(pin, channel)                                                                                                                 \
    (ledcChannelPinCount < maxChannels ? ledcChannelPin[ledcChannelPinCount][0] = (pin), ledcChannelPin[ledcChannelPinCount++][1] = (channel) : 0), \
        ledcAttachPin((pin), (channel))

// Macro to trap values pass to ledcSetup since there is no ESP32 API
#define ledcSetup(channel, freq, resolution)                                                                                                                                                  \
    (ledcChannelResolutionCount < maxChannels ? ledcChannelResolution[ledcChannelResolutionCount][0] = (channel), ledcChannelResolution[ledcChannelResolutionCount++][1] = (resolution) : 0), \
        ledcSetup((channel), (freq), (resolution))

enum pinTypes
{
    digitalPin = 0,
    PWMPin = 1,
    analogPin = 2
};

int getLedcChannelForPin(int pin)
{
    for (int i = 0; i < ledcChannelPinCount; i++)
    {
        if (ledcChannelPin[i][0] == pin)
        {
            return ledcChannelPin[i][1];
        }
    }
    return -1; // Pin not found, return -1 to indicate no channel is associated
}

int mapLedcReadTo8Bit(int channel, uint32_t *originalValue)
{
    uint32_t maxDutyCycle = (1 << channels_resolution[channel]) - 1;
    // Serial.printf("C=%d, R=%d\n",channel,channels_resolution[channel]);
    *originalValue = ledcRead(channel);
    return map(*originalValue, 0, maxDutyCycle, 0, 255);
}

int readGPIO(int gpioNum, uint32_t *originalValue, pinTypes *pintype)
{
    int channel = getLedcChannelForPin(gpioNum);
    int value;
    if (channel != -1)
    {
        // This is a PWM Pin
        value = mapLedcReadTo8Bit(channel, originalValue);
        *pintype = PWMPin;
        return value;
    }
    uint8_t analogChannel = analogGetChannel(gpioNum);
    if (analogChannel != 0 && analogChannel != 255)
    {
        value = mapLedcReadTo8Bit(analogChannel, originalValue);
        *pintype = analogPin;
        return value;
    }
    else
    {
        // This is a digital pin
        *pintype = digitalPin;
        value = digitalRead(gpioNum);
        *originalValue = value;
        if (value == 1)
        {
            return 256;
        }
        return 0;
    }
}

void BluetoothPanel::init() {
    Serial.println("Initializing Bluetooth...");
    SerialBT.begin("ESP32test"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
}

uint32_t originalValue;
pinTypes pintype;
uint32_t lastPinStates[maxGPIOPins];
u_int32_t freeHeap = 0;
u_int32_t freePSRAM = 0;
u_int32_t lastTimePSRAM_sent = 0;

void BluetoothPanel::update() {
    String jsonMessage = "{";
    bool hasChanges = false;

    for (int i = 0; i < maxGPIOPins; i++)
    {
        int currentState = readGPIO(i, &originalValue, &pintype);

        if (originalValue != lastPinStates[i])
        {
            if (hasChanges)
            {
                jsonMessage += ", ";
            }
            jsonMessage += "\"" + String(i) + "\": {\"s\": " + currentState + ", \"v\": " + originalValue + ", \"t\": " + pintype + "}";
            lastPinStates[i] = currentState;
            hasChanges = true;
        }
    }
    
    uint32_t heap = esp_get_free_heap_size();
    if (heap != freeHeap)
    {
        freeHeap = heap;
        jsonMessage += ", \"heap\": " + String(freeHeap);
    }

    if (psramFound())
    {
        uint32_t psram = ESP.getFreePsram();
        if (psram != freePSRAM || lastTimePSRAM_sent > 10)
        {
            freePSRAM = psram;
            lastTimePSRAM_sent = 0;
            Serial.println("HERE");
            jsonMessage += ", \"psram\": " + String(freePSRAM);
        } else {
            lastTimePSRAM_sent++;
        }
    }

    jsonMessage += "}";

    if (hasChanges)
    {
        SerialBT.println(jsonMessage);
        Serial.println(jsonMessage);
    }
}

