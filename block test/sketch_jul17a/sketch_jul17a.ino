#include <math.h>
#include <M5StickCPlus.h>

int user[2] = {0,2};
uint16_t screen[9][16] = { 
			{BLUE,BLUE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
			{BLUE,BLUE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,ORANGE,BLACK,BLACK},
			{BLUE,BLUE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
			{BLUE,BLUE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,YELLOW,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
			{BLUE,BLUE,BLACK,BLACK,BLACK,BLACK,RED,BLACK,YELLOW,YELLOW,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
			{GREEN,GREEN,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
			{GREEN,GREEN,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
			{GREEN,GREEN,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
			{GREEN,GREEN,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
		};

//screen[user[0]][user[1]] = BLUE;

uint16_t old_colour = BLUE;

unsigned long lastTime = 0;

void draw_block(int x, int y, uint16_t colour) {
	M5.lcd.fillRect(x*15,y*15, 15, 15, colour); 
}

void draw_screen() {
	for ( int y=0; y<9; y++ ) {
    for ( int x=0; x<16; x++ ) {
			draw_block(x, y, screen[y][x]);
      //delay(200);
		}
	}
}

void process_input2() {
  
  screen[user[1]][user[0]] = old_colour;
  
  if (M5.BtnA.wasPressed() && M5.BtnB.wasPressed()) {
    user[0] -= 2;
    user[1] -= 2;
  }
  else if (M5.BtnA.wasPressed()) {
    
    user[0] += 1;
  }
  else if (M5.BtnB.wasPressed()) {
    
    user[1] += 1;
  }
  
  
  old_colour = screen[user[1]][user[0]];
	screen[user[1]][user[0]] = BLUE;
  
}

void process_input() {
  
  screen[user[1]][user[0]] = old_colour;
  if (M5.BtnA.wasPressed()) {
    
    user[0] = random(16);
  }
  else if (M5.BtnB.wasPressed()) {
    
    user[1] = random(9);
  }
  
  
  old_colour = screen[user[1]][user[0]];
	screen[user[1]][user[0]] = WHITE;
  
}

void animate(){
  static int x_move = 1;

  unsigned long now = millis();
  screen[user[1]][user[0]] = old_colour;

  if (now - lastTime > 150) {
     user[0] = user[0] + x_move;
     if (user[0] == 15) {x_move = -1;}
     else if (user[0] == 0) {x_move = 1;}
    lastTime = now;
  }
/*  else {
    user[1] = random(9);
  }*/
  
  old_colour = screen[user[1]][user[0]];
	screen[user[1]][user[0]] = WHITE;

  
}

void setup(){
	M5.begin();
  M5.lcd.setRotation(3);
  //screen[0][4] = BLUE;
	M5.lcd.fillScreen(WHITE);
	delay(200);
  lastTime = millis();
  //M5.lcd.fillRect(0,0, 15, 15, RED); 
}


void loop(){
	M5.update();
	animate();
  process_input();
  
	draw_screen();
	//delay(200);
}