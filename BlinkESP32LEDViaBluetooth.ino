#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {

  SerialBT.begin("ESP32 LED"); //set the name of the Bluetooth module

  pinMode(2, OUTPUT); //set GPIO 2 as output

}

void loop() {

  if (SerialBT.available()) { //check if data is available

    char c = SerialBT.read(); //read the data

    if (c == '1') { //if the data is '1'

      digitalWrite(2, HIGH); //turn LED on

      SerialBT.println("LED on");

    }

    if (c == '0') { //if the data is '0'

      digitalWrite(2, LOW); //turn LED off

      SerialBT.println("LED off");

    }

  }

  delay(20); //delay for stability

}
