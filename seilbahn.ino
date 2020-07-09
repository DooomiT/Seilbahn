#define LED 13
#define STEP 11
#define DIR 12
#define TRIGGER1 2
#define ECHO1 4
#define TRIGGER2 7
#define ECHO2 8

//defines variables
long duration;
int distance;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(LED, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(TRIGGER1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIGGER2, OUTPUT);
  pinMode(ECHO2, INPUT);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

//Stringvalue parsing "1:val_1&2:val_2"
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

bool ultrasonic(int *step_value, int *dir_value, int trigger_pin, int echo_pin) {
  // Clears the trigPin
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo_pin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  if (distance <= 20)
  {
    *step_value == 0;
    return false;
  }
  return true;
}

//Sets data of motor inputs with errorhandling
bool setMotorData(int *step_value, int *dir_value) // returns true if calculation was successfull
{
  if (*step_value < 0 | *step_value > 255)
  {
    Serial.print("ERROR: step_value out of range\n");
    analogWrite(STEP, 0); // set if error happend
    return false;
  }
  analogWrite(STEP, *step_value);
  if (*dir_value < 1 | *dir_value > 2)
  {
    Serial.print("ERROR: dir_value out of range\n");
    return false;
  }
  if (*dir_value == 1)
  {
    digitalWrite(DIR, HIGH);
    if(!ultrasonic(step_value, dir_value, TRIGGER1, ECHO1))
    {
        Serial.print("DEBUG: reaching wall stopping engine\n");
    }
  }
  else if (*dir_value == 2)
  {
    digitalWrite(DIR, LOW);
    if(!ultrasonic(step_value, dir_value, TRIGGER2, ECHO2))
    {
        Serial.print("DEBUG:  reaching wall stopping engine\n");
    }
  }
  return true;
}

void loop() {
  if (Serial.available() > 0) {
    digitalWrite(LED, LOW);
    String incommingString = Serial.readString();

    int step_value = getValue(incommingString, ':', 1).toInt();
    int dir_value = getValue(incommingString, ':', 2).toInt();
    Serial.print("received data: ");
    String printString = String(step_value) + "; " + String(dir_value) + "\n";
    Serial.print(printString);
    if (setMotorData(&step_value, &dir_value))
    {
      Serial.print("set data successfull\n");
    }
    else
    {
      Serial.print("ERROR: data could not be set\n");
    }
    Serial.flush();
  }
  else
  {
    digitalWrite(LED, HIGH);
  }
}

