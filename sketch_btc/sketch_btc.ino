const int TRAIN_SPEED_PWM = 3;
const int TRAIN_DIR = 12;

const int DELAY_MILLI_SEC = 10;
const float SPEED_DELTA_PER_SEC = 40.0f;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int reqSpeed = 0;
float curSpeed = 0.0f;

void setup() {
  inputString.reserve(200);
  Serial.begin(9600);
  
  pinMode(TRAIN_SPEED_PWM, OUTPUT);
  pinMode(TRAIN_DIR, OUTPUT);
}

void loop() {
  if (stringComplete) {
    stringComplete = false;

    int index = inputString.indexOf("(");
    if(index != -1) {
      int index2 = inputString.indexOf(")", index);
      if(index2 != -1) {
        String inputCommand = inputString.substring(index+1, index2);
        String resultStr = processInput(inputCommand);
        Serial.println(resultStr);
        inputString = inputString.substring(index2+1);
      }
    }
  }
  
  processSpeedChange();
  delay(DELAY_MILLI_SEC);
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if(inChar == '\n' || inChar == ' ') continue;
    inputString += inChar;
    if (inChar == ')') {
      stringComplete = true;
    }
  }
}

String processInput(String inputStr) {
  inputStr.trim();
  int index = inputStr.indexOf(",");
  if(index == -1) {
    return "Invalid input. error 001. input:("+inputStr+")";
  }
  String command = inputStr.substring(0, index);
  String speed = inputStr.substring(index+1);
  
  processCommand(command, speed.toInt());
  
  return "("+command+","+speed+") OK";
}

void processCommand(String command, int speed) {
  if(command == "s") {
    reqSpeed = speed;
  }
}

void processSpeedChange() {
  if(reqSpeed != int(curSpeed)) {
    if(reqSpeed > curSpeed) {
      curSpeed += (SPEED_DELTA_PER_SEC*DELAY_MILLI_SEC/1000.0f);
    } else {
      curSpeed -= (SPEED_DELTA_PER_SEC*DELAY_MILLI_SEC/1000.0f);
    }
    analogWrite(TRAIN_SPEED_PWM, abs(curSpeed));
    digitalWrite(TRAIN_DIR, curSpeed>=0);
  }
}
