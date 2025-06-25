// a - nMos
// b - pMos
uint32_t delay_time_us = 1000;
const int clka = 26;
const int da = 25;
const int rsta = 12;
const int clkb = 27;
const int db = 33;
const int rstb = 14;
// declarations for shift register pins
// declarations for filter pins








const uint8_t mat_size = 8;
uint8_t mat[mat_size][mat_size];








const uint8_t num_filters = 4;
const uint16_t buf_len = 4; //Hz
uint16_t filter_buf[num_filters][buf_len] = {0};
uint16_t filter_mean[num_filters];
const uint8_t filter_pins[num_filters] = {A3, A6, A7, A4}; // 3 6 7 4
const float filter_scalers[num_filters] = {100.0, 173.0, 214.0, 436.0};
uint8_t currentCol = 0;








hw_timer_t * led_timer = NULL;
portMUX_TYPE timer_mux = portMUX_INITIALIZER_UNLOCKED;








// hardware timer initializations








void reset_matrix() {
digitalWrite(rsta, HIGH);
digitalWrite(rstb,HIGH);
delay(10);
digitalWrite(rsta, LOW);
digitalWrite(rstb, LOW);
}








void shift_one_bit_a(uint8_t d) {
 if(d){
 digitalWrite(da, HIGH);
 }
 else{
 digitalWrite(da, LOW);
 }
 digitalWrite(clka, HIGH);
 delay(1);
 digitalWrite(clka, LOW);
}








void shift_one_bit_b(uint8_t d) {
 if(d){
 digitalWrite(db, HIGH);
 }
 else{
 digitalWrite(db, LOW);
 }
 digitalWrite(clkb, HIGH);
 delay(2);
 digitalWrite(clkb, LOW);
}








void IRAM_ATTR animation_handler() {
 portENTER_CRITICAL_ISR(&timer_mux);
   currentCol = (currentCol + 1) % 8;
   //reset_matrix();
   for(uint8_t c = 0; c < 8; c++) {
     shift_one_bit_b(mat[currentCol][c]); // Vertical
   }
   for(uint8_t r = 0; r < 8; r++){
     if (r == currentCol){
       shift_one_bit_a(1);
     } else {
       shift_one_bit_a(0);
     }
   
   }
 portEXIT_CRITICAL_ISR(&timer_mux);
}








void update_mat(uint8_t * cols, uint8_t num_rows, uint8_t num_cols) {
 for(int r = 0; r < num_rows; r++){
   for(int c = 0; c < num_cols; c++){
     mat[r][c] = 0;
   }
 }




 for(int i = 0; i < num_cols; i++){
   int numberLeft = cols[i];
   for(int k = 8; k > numberLeft-1; k--){
     mat[k][i] = 1;
   }
 }
}
uint16_t calc_buf_mean(uint16_t* buf, uint16_t len) {
 uint32_t total = 0;
 for(uint16_t ii = 0; ii < len; ii++){
   total += buf[ii];
 }
 return total/len;
}




void setup() {
 pinMode(clka, OUTPUT);
 pinMode(da, OUTPUT);
 pinMode(rsta, OUTPUT);




 pinMode(clkb, OUTPUT);
 pinMode(db, OUTPUT);
 pinMode(rstb, OUTPUT);
 // Default pin states
 digitalWrite(clka, LOW);
 digitalWrite(da, LOW);
 digitalWrite(rsta, LOW);




 digitalWrite(clkb, LOW);
 digitalWrite(db, LOW);
 digitalWrite(rstb, LOW);




 led_timer = timerBegin(100000);
 timerAttachInterrupt(led_timer, &animation_handler);
 timerAlarm(led_timer, delay_time_us, true, 0);




 uint8_t sampleMatrix[8] = {1,2,3,4,5,6,7,8};
 update_mat(sampleMatrix, 8, 8);
 Serial.begin(115200);
}








void loop() {
  static uint8_t cols[mat_size];
  static uint16_t counter = 0;
  static unsigned long lastUpdate = 0;
  const unsigned long updateInterval = 10;  // in milliseconds


  // 1. Read analog values and update buffer (every loop iteration)
  for (uint8_t ii = 0; ii < num_filters; ii++) {
    filter_buf[ii][counter] = analogRead(filter_pins[ii]);
  }


  counter = (counter + 1) % buf_len;


  // 2. Only update matrix every `updateInterval` milliseconds
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();


    // 3. Compute moving average for each filter
    for (uint8_t ii = 0; ii < num_filters; ii++) {
      filter_mean[ii] = calc_buf_mean(filter_buf[ii], buf_len);
    }


    // 4. Scale means to matrix height (0 to 8) and store in `cols[]`
    for (uint8_t ii = 0; ii < num_filters; ii++) {
      float scaled = 8.0 * ((float)filter_mean[ii]) / filter_scalers[ii];
      if (scaled > 8.0) scaled = 8.0;
      cols[ii] = (uint8_t)round(scaled);
    }


    // update matrix
  uint8_t cols8[8];
  for (int i = 0; i < num_filters; i++) {
    cols8[i * 2] = cols[i];
    cols8[i * 2 + 1] = cols[i];
  }
  update_mat(cols8, mat_size, mat_size);




    // print filter stuff for finding filter scalars
   // Serial.printf("%4d", filter_mean[0]);
   // for (uint8_t ii = 1; ii < num_filters; ii++) {
   //   Serial.printf(",%4d", filter_mean[ii]);
   // }
   // Serial.println();
  }
}








