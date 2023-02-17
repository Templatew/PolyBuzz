// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 10;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  write_on_sd_card("test.txt", "Hello World!");
}


void write_on_sd_card(String path, String data) {
    File dataFile = SD.open(path, FILE_WRITE);
    if (dataFile) {
        dataFile.println(data);
        dataFile.close();
    }
}

String read_on_sd_card(String path) {
    File dataFile = SD.open(path);
    String data = "";
    if (dataFile) {
        while (dataFile.available()) {
            data += (char) dataFile.read();
        }
        dataFile.close();
    }
    return data;
}

void loop () {

}