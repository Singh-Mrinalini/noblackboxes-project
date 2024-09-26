int LEFT_PWM = 11;
int RIGHT_PWM = 10;

int LEFT_A = 2;
int RIGHT_A = 8;

int LEFT_B = 4;
int RIGHT_B = 7;

int DELAY = 50;

void setup() {
  // Initialize serial port
  Serial.begin(19200);
  
  // LED feedback
  pinMode(LED_BUILTIN, OUTPUT);

  // Digital output pins
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);

  pinMode(LEFT_A, OUTPUT);
  pinMode(RIGHT_A, OUTPUT);

  pinMode(LEFT_B, OUTPUT);
  pinMode(RIGHT_B, OUTPUT);

  // Initialize
  digitalWrite(LEFT_PWM, LOW);
  digitalWrite(RIGHT_PWM, LOW);

  digitalWrite(LEFT_A, LOW);
  digitalWrite(RIGHT_A, LOW);

  digitalWrite(LEFT_B, LOW);
  digitalWrite(RIGHT_B, LOW);
}

void loop() {
  // Check for any incoming bytes
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    // Split the string by '_'
    int index1 = command.indexOf('_');
    String control_mode = command.substring(0, index1);
    
    if (control_mode == "KB") {
      int index2 = command.indexOf('_', index1 + 1);
      String KB_cmd = command.substring(index1 + 1, index2);
      
      // Handle Keyboard Commands
      if (KB_cmd == "f") {
        forward(255);
      } else if (KB_cmd == "b") {
        backward(255);
      } else if (KB_cmd == "l") {
        left();
      } else if (KB_cmd == "r") {
        right();
      } else if (KB_cmd == "s") {
        slow();
      } else if (KB_cmd == "x") {
        stop();
      }
    } else if (control_mode == "GP") {
      // Handle Joystick Commands
      int index2 = command.indexOf('_', index1 + 1);
      float up_down = command.substring(index1 + 1, index2).toFloat();
      float left_right = command.substring(index2 + 1).toFloat();
      
      // Declare control variables at the beginning
      int leftA_val, leftB_val, rightA_val, rightB_val, leftPWM_val, rightPWM_val;

      if (left_right > 0.01 && up_down < -0.01) {
        // Forward right
        leftA_val = HIGH;
        leftB_val = LOW;
        leftPWM_val = int(55 + abs(up_down) * (255 - 55));
        rightPWM_val = int(55 + abs(left_right) * abs(up_down) * (255 - 55));
        if (left_right > 0.5) {
          rightA_val = LOW;
          rightB_val = HIGH;
        } else {
          rightA_val = HIGH;
          rightB_val = LOW;
        }
      } else if (left_right < -0.01 && up_down < -0.01) {
        // Forward left
        rightA_val = HIGH;
        rightB_val = LOW;
        rightPWM_val = int(55 + abs(up_down) * (255 - 55));
        leftPWM_val = int(55 + abs(left_right) * abs(up_down) * (255 - 55));
        if (left_right < -0.5) {
          leftA_val = LOW;
          leftB_val = HIGH;
        } else {
          leftA_val = HIGH;
          leftB_val = LOW;
        }
      } else if (left_right > 0.01 && up_down > 0.01) {
        // Backward right
        leftA_val = LOW;
        leftB_val = HIGH;
        leftPWM_val = int(55 + abs(up_down) * (255 - 55));
        rightPWM_val = int(55 + abs(left_right) * abs(up_down) * (255 - 55));
        if (left_right > 0.5) {
          rightA_val = HIGH;
          rightB_val = LOW;
        } else {
          rightA_val = LOW;
          rightB_val = HIGH;
        }
      } else if (left_right < 0.01 && up_down > 0.01) {
        // Backward left
        rightA_val = LOW;
        rightB_val = HIGH;
        rightPWM_val = int(55 + abs(up_down) * (255 - 55));
        leftPWM_val = int(55 + abs(left_right) * abs(up_down) * (255 - 55));
        if (left_right < -0.5) {
          leftA_val = HIGH;
          leftB_val = LOW;
        } else {
          leftA_val = LOW;
          leftB_val = HIGH;
        }
      } else if (up_down <= 0.01 && up_down >= -0.01) {
        // stop
        leftA_val = LOW;
        leftB_val = LOW;
        rightA_val = LOW;
        rightB_val = LOW;
        leftPWM_val = LOW;
        rightPWM_val = LOW;
      } else if (left_right <= 0.01 && left_right >= -0.01 && up_down > 0.01) {
        // backward
        leftA_val = LOW;
        leftB_val = HIGH;
        rightA_val = LOW;
        rightB_val = HIGH;
        leftPWM_val = int(55 + abs(up_down) * (255 - 55));
        rightPWM_val = int(55 + abs(up_down) * (255 - 55));
      } else if (left_right <= 0.01 && left_right >= -0.01 && up_down < 0.01) {
        // forward
        leftA_val = HIGH;
        leftB_val = LOW;
        rightA_val = HIGH;
        rightB_val = LOW;
        leftPWM_val = int(55 + abs(up_down) * (255 - 55));
        rightPWM_val = int(55 + abs(up_down) * (255 - 55));
      }
      
      // Call control with the set values
      control(leftA_val, leftB_val, rightA_val, rightB_val, leftPWM_val, rightPWM_val);
    }
  }

  // Add delay
  delay(DELAY);
}

void control(int leftA_val, int leftB_val, int rightA_val, int rightB_val, int leftPWM_val, int rightPWM_val) {
  digitalWrite(LED_BUILTIN, HIGH);

  digitalWrite(LEFT_A, leftA_val);
  digitalWrite(LEFT_B, leftB_val);

  digitalWrite(RIGHT_A, rightA_val);
  digitalWrite(RIGHT_B, rightB_val);

  analogWrite(LEFT_PWM, leftPWM_val);
  analogWrite(RIGHT_PWM, rightPWM_val);
}

void go(int value) {
  analogWrite(LED_BUILTIN, value);
  analogWrite(LEFT_PWM, value);
  analogWrite(RIGHT_PWM, value);
}

void slow() {
  digitalWrite(LEFT_A, HIGH);
  digitalWrite(LEFT_B, LOW);
  digitalWrite(RIGHT_A, HIGH);
  digitalWrite(RIGHT_B, LOW);

  for (int i = 75; i >= 0; i--) {
    analogWrite(LED_BUILTIN, i);
    analogWrite(LEFT_PWM, i);
    analogWrite(RIGHT_PWM, i);
    delay(DELAY);
    if (Serial.available() > 0) {
      break;
    }
  }
}

void forward(int value) {
  digitalWrite(LEFT_A, HIGH);
  digitalWrite(LEFT_B, LOW);
  digitalWrite(RIGHT_A, HIGH);
  digitalWrite(RIGHT_B, LOW);
  go(value);
}

void backward(int value) {
  digitalWrite(LEFT_A, LOW);
  digitalWrite(LEFT_B, HIGH);
  digitalWrite(RIGHT_A, LOW);
  digitalWrite(RIGHT_B, HIGH);
  go(value);
}

void left() {
  digitalWrite(LEFT_A, LOW);
  digitalWrite(LEFT_B, HIGH);
  digitalWrite(RIGHT_A, HIGH);
  digitalWrite(RIGHT_B, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  analogWrite(LEFT_PWM, 255);
  analogWrite(RIGHT_PWM, 120);
}

void right() {
  digitalWrite(LEFT_A, HIGH);
  digitalWrite(LEFT_B, LOW);
  digitalWrite(RIGHT_A, LOW);
  digitalWrite(RIGHT_B, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  analogWrite(LEFT_PWM, 120);
  analogWrite(RIGHT_PWM, 255);
}

void stop() {
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LEFT_PWM, LOW);
  digitalWrite(RIGHT_PWM, LOW);
  digitalWrite(LEFT_A, LOW);
  digitalWrite(RIGHT_A, LOW);
  digitalWrite(LEFT_B, LOW);
  digitalWrite(RIGHT_B, LOW);
}
