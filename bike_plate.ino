/**
 * Freddie's bike number plate control
 */

#define TOGGLE_SWITCH   2
#define LIMIT_TOP       7
#define LIMIT_BOTTOM    8
#define MOTOR1_1        3
#define MOTOR1_2        4
#define MOTOR_ENABLE    5
#define STATUS_LED      6

#define STATE_STOPPED   0
#define STATE_OPENING   1
#define STATE_CLOSING   2

boolean is_open, is_closed;
int state;

void setup() {
  Serial.begin(9600);
  
  // inputs
  pinMode(TOGGLE_SWITCH, INPUT);
  digitalWrite(TOGGLE_SWITCH, HIGH); // internal pull-up
  pinMode(LIMIT_TOP, INPUT);
  digitalWrite(LIMIT_TOP, HIGH); // internal pull-up
  pinMode(LIMIT_BOTTOM, INPUT);
  digitalWrite(LIMIT_BOTTOM, HIGH); // internal pull-up

  // outputs
  pinMode(MOTOR1_1, OUTPUT);
  pinMode(MOTOR1_2, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(STATUS_LED, LOW);
  
  state = STATE_STOPPED;
}

// read the state of the limit switches
void readLimitSwitches() {
  is_open = (digitalRead(LIMIT_TOP) == HIGH ? false : true);
  is_closed = (digitalRead(LIMIT_BOTTOM) == HIGH ? false : true);  
  
  //Serial.print("is open: ");
  //Serial.println(is_open);
  //Serial.print("is closed: ");
  //Serial.println(is_closed);
  
  // update status led
  if (!is_open) digitalWrite(STATUS_LED, HIGH);
  else digitalWrite(STATUS_LED, LOW);
}

void readToggleSwitch() {
  if (state == STATE_STOPPED) {
    if (digitalRead(TOGGLE_SWITCH) != HIGH) {
      // toggle button pressed
      if (is_open) state = STATE_CLOSING;
      else state = STATE_OPENING;
    }
  }
}

void loop() {
  readLimitSwitches();
  readToggleSwitch();
  
  //delay(1000);
  
  //Serial.println("================================");
  //Serial.print("state: ");
  //Serial.println(state);
  
  if (state == STATE_STOPPED) {
    Serial.println("stopped");
    if  (!is_open && !is_closed) {
      //Serial.println("stopped but not open, so opening");
      // number plate is neither open or closed, let's open it for safety
      state = STATE_OPENING;
    } else {
      //Serial.println("switching motor off");
      // switch motor off
      digitalWrite(MOTOR1_1, LOW);
      digitalWrite(MOTOR1_2, LOW);
      digitalWrite(MOTOR_ENABLE, LOW);
    }
  } 
  
  if (state == STATE_OPENING && !is_open) {
    // make sure motor is turning to open the number plate
    digitalWrite(MOTOR1_1, HIGH);
    digitalWrite(MOTOR1_2, LOW);
    digitalWrite(MOTOR_ENABLE, HIGH);
  }

  if (state == STATE_CLOSING && !is_closed) {
    // make sure motor is turning to close the number plate
    digitalWrite(MOTOR1_1, LOW);
    digitalWrite(MOTOR1_2, HIGH);
    digitalWrite(MOTOR_ENABLE, HIGH);
  }

  if ((state == STATE_OPENING && is_open)
      || (state == STATE_CLOSING && is_closed)) {
    // reached it's limit
    state = STATE_STOPPED;
    delay(1000); // crude debounce - let limit switches settle
  }
  
}
