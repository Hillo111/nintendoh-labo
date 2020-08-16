#include <LiquidCrystal.h>
#include <EEPROM.h>

const int rs = 52, en = 53, d4 = 48, d5 = 49, d6 = 50, d7 = 51;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

  
const int button = 23, buzz = 22;

byte dino1[8] = {
  B11111,
  B10100,
  B11111,
  B00110,
  B10111,
  B11110,
  B01110,
  B00010,
};

byte dino2[8] = {
  B11111,
  B10100,
  B11111,
  B00110,
  B10111,
  B11110,
  B01110,
  B01000,
};

byte cactus[8] = {
  B00100,
  B00101,
  B00101,
  B10101,
  B10111,
  B11100,
  B00100,
  B00100,
};

byte bird1[8] = {
  B00000,
  B01000,
  B11000,
  B00111,
  B00111,
  B01100,
  B01100,
  B01000,
};


byte bird2[8] = {
  B00000,
  B00001,
  B01011,
  B11011,
  B00111,
  B00111,
  B00000,
  B00000,
};

int jump = 0, x = 2;
int jump_power = 4;

bool birds[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
bool cacti[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

const int speed_up = 12;

const int point_increase = 5;
const int speed_up_on = point_increase * 8;

bool state = false;
bool death = false;

const int min_speed = 100, start_speed = 500;

int will_spawn = 0;
bool to_spawn = false;
bool last_spawned = true;

double dif = pow(2, 1.0/12.0);

unsigned long highscore = 0;

const int jump_tone = 620, death_tones[4] = {440, 440 / pow(dif, 1), 440 / pow(dif, 2), 440 / pow(dif, 3)}, init_death = 220, highscore_tone [6] = {440, 440 * pow(dif, 1), 440 * pow(dif, 2), 440 * pow(dif, 4), 440 * pow(dif, 2), 440 * pow(dif, 4)};

const int SPAWN_PATTERN_COUNT = 6;
const int SPAWN_PATTERN_LENGTH = 6;
bool spawn_patterns[SPAWN_PATTERN_COUNT][2][SPAWN_PATTERN_LENGTH] = {{{false, false, false, false, true, true}, {false, true, true, false, false, false}},
{{false, false, false, false, false, true}, {false, false, true, true, false, false}},
{{false, false, false, false, false, false}, {false, false, true, true, false, true}}, {{false, false, false, false, false, false
}, {true, false, true, true, false, false}},
{{false, false, false, false, false, false}, {false, true, false, true, true, false}}, {{false, false, false, false, true, false}, {false, true, true, false, false, true}}};

const int new_pattern_on = SPAWN_PATTERN_LENGTH * point_increase;

const int NEEDED_SPACING = 5;

void dino_setup() {
  lcd.createChar(0, dino1);
  lcd.createChar(1, dino2);
  lcd.createChar(5, cactus);
  lcd.createChar(3, bird1);
  lcd.createChar(4, bird2);
  
  lcd.clear();
  lcd.print("Reading from");
  lcd.setCursor(0, 1);
  lcd.print("EEPROM...");
  lcd.setCursor(0, 0);

  Serial.begin(9600);
  // Serial.println(to_dec("10000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));

  String r = "";
  for (int i = 0; i < 2000; i ++){
    r += String(EEPROM.read(i));
    // EEPROM.write(i, 0);
  }
  Serial.println(r);
  highscore = to_dec(r);

}
unsigned long seed = analogRead(0);
int dino_loop() {
  // initial menu
  randomSeed(seed);

  seed = 0;
  
  draw_until_press("Highscore:\n" + String(highscore), "Press to start\nHold to exit", 1600);

  // int new_seed = 0;
  // wait for release
  lcd.clear();
  int co = 0;
  while (digitalRead(button) == 1){
    seed ++;
    co ++;
    delay(1);
    if (co > 1000){
      return 1;
    }
  }

  // reset everything 
  unsigned long counter = 0;
  unsigned long points = 0;

  int current_pattern_index = random(0, SPAWN_PATTERN_COUNT);
  int current_index = 5;

  int last_pattern_index = current_pattern_index;
  
  bool will_jump = false;

  jump = 0;

  int update_speed = start_speed;

  for (int i = 0; i < 16; i ++){
    birds[i] = false;
    cacti[i] = false;
  }

  int step_counter = 0;

  int needed_spacing = 0;

  int c_counter = 0;

  // main game loop
  while (!death){
    // buffer jump
    if (digitalRead(button) == 1){
      will_jump = true;
    }

    // the frame on which we need to update
    if (counter == update_speed - 1){
      // check if we need to spawn something
      if (current_index == SPAWN_PATTERN_LENGTH){
        needed_spacing = NEEDED_SPACING;
        for (int i = SPAWN_PATTERN_LENGTH - 1; i >= 0; i --){
          if (spawn_patterns[current_pattern_index][1][i] || spawn_patterns[current_pattern_index][0][i]){
            break;
          }
          needed_spacing --;
        }
        last_pattern_index = current_pattern_index;
        while (current_pattern_index == last_pattern_index){
          current_pattern_index = random(0, SPAWN_PATTERN_COUNT);
        }
        
        for (int i = 0; i < SPAWN_PATTERN_LENGTH; i ++){
          if (spawn_patterns[current_pattern_index][1][i] || spawn_patterns[current_pattern_index][0][i]){
            break;
          }
          needed_spacing --;
        }
        
        current_index = 0;
        Serial.println(current_pattern_index);
      }
      if (needed_spacing <= 0){
        birds[15] = spawn_patterns[current_pattern_index][0][current_index];
        cacti[15] = spawn_patterns[current_pattern_index][1][current_index];
        current_index += 1;
      }
      else {
        needed_spacing -= 1;
      }
      
      /*
      if (will_spawn <= 0){
        
        if (to_spawn){ // cactus
          cacti[15] = true;
          c_counter += 1;
        }
        else { // bird
          birds[15] = true;
          c_counter = 0;
        }
        last_spawn = to_spawn;

        // if cactus count < 3 we can place a cactus or a bird
 
        if (c_counter < 3){
          int next_place = rand() % 2; // what we will place next

          // check so that we don't place 2 birds in a row
          if (last_spawn){ // cactus
            if (next_place == 0){ // bird
              to_spawn = false;
              will_spawn = rand() % (max_dis - min_dis) + (min_dis);
            }
            else { // cactus
              to_spawn = true; 
              will_spawn = 0; // next loc
            }
          }
          else { // bird
            to_spawn = true; // cactus
            will_spawn = rand() % (max_dis - min_dis) + (min_dis);
          }
          
        }
        // if cactus count >= 3 we can only spawn a bird
        else {
          to_spawn = false;
          will_spawn = rand() % (max_dis - min_dis) + (min_dis);
        }
      }
      */
      
      // will_spawn -= 1;
      
      if (will_jump && jump == 0){
        tone(buzz, jump_tone, 100);
        jump = jump_power;
      }
      if (will_jump){
        will_jump = false;
      }
      update_game();
      lcd.clear();
      draw();

      points += point_increase;
      
      if (points % speed_up_on == 0){
        update_speed -= speed_up;
      }
      if (update_speed < min_speed){
        update_speed = min_speed;
      }
      
    }
    seed ++;
 
    counter ++;
    counter %= update_speed;
    delay(1);
  }

  tone(buzz, init_death, 400);
  delay(500);
  for (int i = 0; i < 6; i ++){
    tone(buzz, death_tones[i], 200);
    delay(200);
  }

  lcd.clear();
  if (points > highscore){
    // write to eeprom to save highscore
    String w = to_bin(points);
    Serial.println(w);
    for (int i = 0; i < w.length(); i ++){
      EEPROM.write(i, (w[i] - '0'));
    }
    
    lcd.setCursor(0, 0);
    lcd.print("New highscore!");
    
    for (int i = 0; i < 6; i ++){
      tone(buzz, highscore_tone[i], 200);
      delay(200);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    highscore = points;   
    draw_until_press("New highscore!\n" + String(points), "Press to\ncontinue", 1600);
  }
  else {
    lcd.setCursor(0, 0);
    draw_until_press("Final score:\n" + String(points), "Press to\ncontinue", 1600);
  }


  death = false;
  return 0;
}

void update_game(){
  
  for (int i = 0; i < 15; i ++){
    birds[i] = birds[i + 1];
    // Serial.println(String(i) + ' ' + String(birds[i]) + ' ' + String(birds[i + 1]));
    cacti[i] = cacti[i + 1];
    // Serial.println(cacti[i]);
  }
  birds[15] = false;
  cacti[15] = false;

  if (birds[x] && jump > 0){
    death = true;
    return ;
  }
  
  if (jump > 0){
    jump --;
  }
  
  if (cacti[x] && jump == 0){
    death = true;
    return ;
  }
  state = not state;
}

void draw(){
  for (int i = 0; i < 16; i ++){
    if (birds[i]){
      lcd.setCursor(i, 0);
      if (state){
        lcd.write(byte(3));
      }
      else {
        lcd.write(byte(4));
      }
    }
    if (cacti[i]){
      // Serial.println("drew cactus");
      lcd.setCursor(i, 1);
      lcd.write(byte(5));
    }
  }
  int y;
  if (jump > 0){
    y = 0;
  }
  else {
    y = 1;
  }
  lcd.setCursor(x, y);
  if (state){
    lcd.write(byte(0));
  }
  else {
    lcd.write(byte(1));
  }
}

void draw_until_press(String s1, String s2, int switch_time){
  bool rel = false;
  int c = 0;
  
  while (!(rel && digitalRead(button) == 1)){
    seed ++;
    if (c == 0){
      lcd.clear();
      for (int i = 0; i < s1.length(); i ++){
        if (s1[i] != '\n'){
          lcd.print(s1[i]);
        }
        else{
          lcd.setCursor(0, 1);
        }
      }
      lcd.setCursor(0, 0);
    }
    if (c == switch_time){
      lcd.clear();
      for (int i = 0; i < s2.length(); i ++){
        if (s2[i] != '\n'){
          lcd.print(s2[i]);
        }
        else{
          lcd.setCursor(0, 1);
        }
      }
      lcd.setCursor(0, 0);
    }

    if (digitalRead(button) == 0){
      rel = true;
    }
    c ++;
    c %= switch_time * 2;
    delay(1);
  }
}

String to_bin(int dec){
  String val = "";
  while (dec > 0){
    val += String(dec % 2);
    dec >>= 1;
  }
  return val;
   
}
int to_dec(String bin){
  int val = 0;
  for (int i = 0; i < bin.length(); i ++){
    if (bin[i] == '1'){
      val += ceil(pow(2, i ));
    }
  }
  return val;
}

void dinosaur_game(){

  dino_setup();
  while (true){
    int ret_val = dino_loop();
    if (ret_val == 1){
      break;
    }
  }
}




/*

bool released = false;

int a_highscore = 0;

byte base1[8] = {
  B00100,
  B01110,
  B11110,
  B11111,
  B11110,
  B11110,
  B11110,
  B11110,
};

byte base2[8] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11111,
  B11110,
  B01110,
  B00100,
};




void alien_setup(){
  lcd.createChar(0, base1);
  lcd.createChar(1, base2);
  lcd.clear();
  lcd.setCursor(15, 0);
  lcd.write(byte(0));
  lcd.setCursor(15, 1);
  lcd.write(byte(1));
  released = false;
}

int alien_loop(){
  draw_until_press("Highscore:\n" + String(a_highscore));
  if (digitalRead(button) == 1){
    t ++;
  }
  else {
    released = true;
    t = 0;
  }
  delay(1);
  if (t > 1000 && released){
    return 1;
  }
  return 0;
}


void alien_game(){
  alien_setup();
  while (true){
    int ret_val = alien_loop();
    if (ret_val == 1){
      break;
    }
  }
}


*/






const int game_count = 1;
String games[game_count] = {"Dinosaur jump"};

const int select_tone = 550, select_length = 150;

byte arrow[8] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B11111,
  B00110,
  B00100,
  B00000,
};

int scroll_until_press(String message, int scroll_speed){
  int hold_counter = 0;
  int index = 0;
  
  while (true){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.write(byte(0));
    lcd.print(" " + String(games[index]));
    lcd.setCursor(0, 0);
    for (int i = 0; i < scroll_speed * 10; i ++){
      if (digitalRead(button) == 1){
        hold_counter ++;
      }
      else {
        if (hold_counter < 1000 && hold_counter > 0){
          tone(buzz, select_tone, select_length);
          index += 1;
          if (index > game_count - 1){
            index = 0;
          }
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.write(byte(0));
          lcd.print(" " + String(games[index]));
          lcd.setCursor(0, 0);
        }
        hold_counter = 0;
      }
      if (hold_counter > 1000){
        return index;
      }
      
      delay(1);
    }
    String sub_str = message;
    while (sub_str.length() > 16){
      String new_substr = "";
      for (int i = 1; i < sub_str.length(); i ++){
        new_substr += sub_str[i];
      }
      sub_str = new_substr;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(sub_str);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
      lcd.print(" " + String(games[index]));
      lcd.setCursor(0, 0);
      for (int i = 0; i < scroll_speed; i ++){
        if (digitalRead(button) == 1){
        hold_counter ++;
      }
      else {
        if (hold_counter < 1000 && hold_counter > 0){
          tone(buzz, select_tone, select_length);
          index += 1;
          if (index > game_count - 1){
            index = 0;
          }
          lcd.setCursor(0, 0);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.write(byte(0));
          lcd.print(" " + String(games[index]));
          lcd.setCursor(0, 0);
        }
        hold_counter = 0;
      }
      if (hold_counter > 1000){
        return index;
      }
      
      delay(1);
      }
    }
  }
}

void setup(){
  lcd.begin(16, 2);
}

void loop(){
  lcd.createChar(0, arrow);
  int i = scroll_until_press("Welcome player! Select a game by pressing the button. Enter a game by holding the button.", 200);
  if (i == 0){
    dinosaur_game();
  }
  if (i == 1){
    //alien_game();
  }
  
  
}