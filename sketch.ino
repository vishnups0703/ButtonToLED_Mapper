// ESP32 - Struct-based button to LED mode switching

#define BUTTON1_PIN 2
#define BUTTON2_PIN 14
#define LED1_PIN 21
#define LED2_PIN 19

// Timing
const unsigned long interval = 500; // ms
unsigned long lastActionTime = 0;
bool ledState = false;

// Mode definitions
enum Mode {
    MODE_NONE,
    MODE_TOGGLE_ALTERNATE,
    MODE_BLINK_TOGETHER
};

Mode currentMode = MODE_NONE;

// Struct for button-mode mapping
typedef struct {
    uint8_t buttonPin;
    Mode mappedMode;
} ButtonMap;

// Button-to-mode map
ButtonMap buttonMap[] = {
    {BUTTON1_PIN, MODE_TOGGLE_ALTERNATE},
    {BUTTON2_PIN, MODE_BLINK_TOGETHER}
};

#define NUM_BUTTONS (sizeof(buttonMap) / sizeof(ButtonMap))

// Store last button states
bool lastButtonState[NUM_BUTTONS];

// Setup
void setup() {
    // LEDs
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);

    // Buttons
    for (int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(buttonMap[i].buttonPin, INPUT_PULLUP);
        lastButtonState[i] = HIGH; // default unpressed
    }

    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
}

// Main loop
void loop() {
    checkButtons();
    runCurrentMode();
}

// Handle button presses via struct
void checkButtons() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        bool currentState = digitalRead(buttonMap[i].buttonPin);

        if (lastButtonState[i] == HIGH && currentState == LOW) {
            // Button just pressed
            if (currentMode == buttonMap[i].mappedMode) {
                currentMode = MODE_NONE;  // Toggle OFF
                digitalWrite(LED1_PIN, LOW);
                digitalWrite(LED2_PIN, LOW);
            } else {
                currentMode = buttonMap[i].mappedMode;  // Activate mode
                ledState = false;
                lastActionTime = millis();
            }
        }

        lastButtonState[i] = currentState;
    }
}

// Run LED behavior based on current mode
void runCurrentMode() {
    if (currentMode == MODE_NONE)
        return;

    if (millis() - lastActionTime >= interval) {
        lastActionTime = millis();
        ledState = !ledState;

        if (currentMode == MODE_TOGGLE_ALTERNATE) {
            digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
            digitalWrite(LED2_PIN, ledState ? LOW : HIGH);
        } else if (currentMode == MODE_BLINK_TOGETHER) {
            digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
            digitalWrite(LED2_PIN, ledState ? HIGH : LOW);
        }
    }
}
