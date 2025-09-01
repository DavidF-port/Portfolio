# Useless Box – Headstrong Personality

A microcontroller-driven "useless box" that flips a switch back after the user flips it with a headstrong personality.  
The headstrong personality makes the motor rush forward aggressively to switch the lever, then retreat slowly to its resting position, simluating a headstrong personality.

## Features
- Headstrong Personality – Quick strike forward, slow retreat back.  
- PWM + Timing Control – Creates distinct movement patterns to simulate personality.  
- Limit Switch Protection – Ensures safe motor travel.  
- Compact Design – All components fit into a small enclosure.  


## Demo w/ Headstrong Personality
(https://youtu.be/gd_1D4p8ltQ)


## How It Works
1. The user flips a DPDT toggle switch.  
2. The motor arm quickly rushes forward to flip it back.  
3. The arm then slowly retreats using a different timing pattern.  
4. A limit switch prevents the arm from wasting power by only allowing the robot arm to go forward when the switch is pressed.  

---

## Code Snippet
```
const uint8_t D1_pin = 33;
const uint8_t D2_pin = 26;
const uint8_t limit_pin = 27;
const uint8_t dpdt_pin = 32;

void setup() {
  Serial.begin(115200);
  pinMode(D1_pin, OUTPUT);
  pinMode(D2_pin, OUTPUT);
  pinMode(limit_pin, INPUT);
  pinMode(dpdt_pin, INPUT);
}

void loop() {
  uint8_t dpdt = digitalRead(dpdt_pin);
  uint8_t limit = digitalRead(limit_pin);

  if (limit == 0 && dpdt == 1) {
    digitalWrite(D1_pin, LOW);
    digitalWrite(D2_pin, LOW);
    delay(100);
    return;
  }
  else if (dpdt == 1) {
    // Forward: aggressive, fast motion
    digitalWrite(D1_pin, LOW);
    digitalWrite(D2_pin, HIGH);
    delayMicroseconds(500);
    digitalWrite(D2_pin, LOW);
    delayMicroseconds(2000);
  } else {
    // Retreat: slower, deliberate motion
    digitalWrite(D2_pin, LOW);
    digitalWrite(D1_pin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(D1_pin, LOW);
    delayMicroseconds(500);
  }
}
