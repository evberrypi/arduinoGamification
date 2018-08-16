/***************************************************************************************
    Name    : Arduino Gamification Health Challenge
    Author  : Everett Lacey 
    Created : Aug 14, 2018
    Last Modified: Aug 15, 2018
    Version : 1.0
    Notes   : This code is for use with an Arduino Uno and LCD/button shield. The
              game is a two player challenge with hard coded preset goals (eg, drink
              water, do X pushups per day, walk 10k steps). 

    License : This program is free software. You can redistribute it and/or modify
              it under the terms of the GNU General Public License as published by
              the Free Software Foundation, either version 3 of the License, or
              (at your option) any later version.
              This program is distributed in the hope that it will be useful,
              but WITHOUT ANY WARRANTY; without even the implied warranty of
              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
              GNU General Public License for more details.
 ***************************************************************************************/
#include <LiquidCrystal.h>

// log all the points!
int points;
int water = 0;
int pushups = 0;
int steps = 000;
int workout = 0;

int jpoints;
int jworkout = 0;
int jwater = 0;
int jpushups = 0;
int jsteps = 000;


#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int read_LCD_buttons(){
  
  int adc_key_in = analogRead(0);      // read the value from the sensor 
 if (adc_key_in > 1100) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 0)   return btnRIGHT;  
 if (adc_key_in < 100)  return btnUP; 
 if (adc_key_in < 258)  return btnDOWN; 
 if (adc_key_in < 451)return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;  // was 980, some samples are 780/90 (770 and 780 didnt work)
  return btnNONE;  // when all others fail, return this...
}

// You can have up to 10 menu items in the menuItems[] array below without having to change the base programming much.
//  Beyond 10 items, you will have to add additional "cases" in the switch/case section of the operateMainMenu() function below. 
// Any new menu item will reuire additional void functions (i.e. menuItem6, menuItem7, etc.) to the program.
String menuItems[] = {"Stats", "Water", "Pushups", "Steps", "Run/HIIT"};

// Navigation button variables
int readKey;

// Menu control variables
int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
int cursorPosition = 0;

// Creates 3 custom characters for the menu display
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};



// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {

  // Initializes serial communication
  Serial.begin(9600);

  // Initializes and clears the LCD screen
  lcd.begin(16, 2);
  lcd.clear();

  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
}

void loop() {
  doMath();
  doJMath();
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
   int buttonState = read_LCD_buttons();
}

void doMath() {
  int waterMath;
  int workoutMath;
  int stepMath;
  int pushupMath;
  if (pushups < 30) {;
    pushupMath = 0 ;
  } else if (pushups == 30) {
    pushupMath = 2500;
    } else if (pushups >= 70) {
      pushupMath = 5000;
    } else if (pushups >= 100) {
      pushupMath = 10000;
    }
  if (steps <= 1500) {
    stepMath = 0;
  } else if (steps >= 7000) {
    stepMath = 7000;
  } else if (steps >= 10000) {
    stepMath = 15000;
  }
  if (workout == 0) {
    workoutMath = 0;
  } else if( workout == 1) {
    workoutMath = 10000;
  } else if (workout > 1){
    workoutMath = 20000;
  }
  if ( water >= 8 ) {
    waterMath = 5000; 
  } else {
    waterMath = water;
  }

  points = ( pushupMath + stepMath + workoutMath + ((waterMath) * 100));
}
void doJMath() {
    int waterMath;
  int workoutMath;
  int stepMath;
  int pushupMath;
  if (jpushups < 10) {
    pushupMath = 0;
    } else if (jpushups == 15) {
      pushupMath = 2500;
    } else if (jpushups >= 30) {
      pushupMath = 5000;
    } else if (jpushups >= 50) {
      pushupMath = 10000;
    }
  if (jsteps <= 1500) {
    stepMath = 0;
  } else if (jsteps >= 7000) {
    stepMath = 7000;
  } else if (jsteps >= 10000) {
    stepMath = 15000;
  }
  if (jworkout == 0) {
    workoutMath = 0;
  } else if( jworkout == 1) {
    workoutMath = 10000;
  } else if (jworkout > 1){
    workoutMath = 20000;
  }
  if ( jwater >= 8 ) {
    waterMath = 5000; 
  } else {
    waterMath = jwater;
  }

  jpoints = ( pushupMath + stepMath + workoutMath + ((waterMath) * 100));

}
// This function will generate the 2 menu items that can fit on the screen. 
//They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
void mainMenuDraw() {
  Serial.print(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}

// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {     // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  // The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so
  // in order to determine where the cursor should be you need to see if you are at an odd or even menu page and an odd or even cursor position.
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}


void operateMainMenu() {
  int activeButton = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(200);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0: // When button returns as 0 there is no action taken
        break;
      case 1:  // This case will execute if the "forward" button is pressed
        button = 0;
        switch (cursorPosition) { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
          case 0:
            menuItem1();
            break;
          case 1:
            menuItem2();
            break;
          case 2:
            menuItem3();
            break;
          case 3:
            menuItem4();
            break;
          case 4:
            menuItem5();
            break;
//If you need to add new menuItems, add additional cases here like so:

//          case 5:
//            menuItem6();
//            break;
//          case 6:
//            menuItem7();
//            break;
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
      case 2:
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case 3:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
    }
  }
}

// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
int evaluateButton(int x) {
  int result = 0;
  if (x < 50) {
    result = 1; // right
  } else if (x < 250) {
    result = 2; // up
  } else if (x < 350) {
    result = 3; // down
  } else if (x < 450) {
     Serial.println("4");
    result = 4; // left
  }
    else if (x < 700) {
      Serial.println("5");
    result = 5; // select
  }
  return result;
}

// If there are common usage instructions on more than 1 of your menu items you can call this function from the sub
// menus to make things a little more simplified. If you don't have common instructions or verbage on multiple menus
// I would just delete this void. You must also delete the drawInstructions()function calls from your sub menu functions.
void drawInstructions() {
  lcd.setCursor(0, 1); // Set cursor to the bottom line
  lcd.print("Use ");
  lcd.print(byte(1)); // Up arrow
  lcd.print("/");
  lcd.print(byte(2)); // Down arrow
  lcd.print(" buttons");
}

void menuItem1() { // Function executes when you select the 1st item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ev: ");
  lcd.print(points);

  lcd.setCursor(0, 1);
  lcd.print("J : ");
  lcd.print(jpoints);
  
//   for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
//    // scroll one position left:
//    lcd.scrollDisplayLeft();
//    // wait a bit:
//    delay(150);
//  }

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
    }
  }
}

void menuItem2() { // Function executes when you select the 2nd item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Water Count:");
  lcd.setCursor(0,1);
  lcd.print("E:");
  lcd.print(water);
  
  lcd.setCursor(8,1);
  lcd.print("J:");
  lcd.print(jwater);
  int toggleuser = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (toggleuser == 1) {
      if (readKey > 50 && readKey < 250) {
        delay(400);
        lcd.setCursor(2, 1);
        water = water +1;
        lcd.print(water);
        } else if (readKey > 250 && readKey < 400 ) {
          delay(400);
          lcd.setCursor(2, 1);
          water = water - 1;
          lcd.print(water);
        }
      }
      if (toggleuser == 0) {
        if (readKey > 50 && readKey < 250) {
          delay(400);
          lcd.setCursor(10, 1);
           jwater = jwater +1;
           lcd.print(jwater);
        } else if (readKey > 250 && readKey < 400 ) {
          delay(400);
          lcd.setCursor(10, 1);
          jwater = jwater -1;
          lcd.print(jwater);
        }
      }
      if (readKey > 400 && readKey < 450) {
        delay(100);
        readKey = analogRead(0);
        button = evaluateButton(readKey);
        switch (button) {
          case 4:  // This case will execute if the "back" button is pressed
          button = 0;
          activeButton = 1;
          break;
         }
    } 
    if (readKey < 650 && readKey > 639){
      switch(toggleuser) {
        case 0:
        delay(350);
        blinkev();
        toggleuser = 1;
        break;
        case 1:
        delay(350);
        blinkj();
        toggleuser = 0;
        break;
        };
     };
  }  
}

void blinkev() {
        Serial.println("EV");
        lcd.setCursor(0,1);
        lcd.print("_");
        delay(500);
        lcd.setCursor(0,1);
        lcd.print("E");
        delay(300);
        lcd.setCursor(0,1);
        lcd.print("_");
        delay(500);
        lcd.setCursor(0,1);
        lcd.print("E");
     
}
void blinkj() {
        Serial.println("J");
        lcd.setCursor(8,1);
        lcd.print("_");
        delay(500);
        lcd.setCursor(8,1);
        lcd.print("J");
        delay(300);
        lcd.setCursor(8,1);
        lcd.print("_");
        delay(500);
        lcd.setCursor(8,1);
        lcd.print("J");
}

void menuItem3() { // Function executes when you select the 3rd item from main menu
 int activeButton = 0;

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Pushups:");
  lcd.setCursor(0,1);
  lcd.print("E:");
  lcd.print(pushups);
  
  lcd.setCursor(8,1);
  lcd.print("J:");
  lcd.print(jpushups);
  int toggleuser = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (toggleuser == 1) {
      if (readKey > 50 && readKey < 250) {
        delay(400);
        lcd.setCursor(2, 1);
        pushups = pushups +5;
        lcd.print(pushups);
        } else if (readKey > 250 && readKey < 400 ) {
          delay(400);
          lcd.setCursor(2, 1);
          pushups = pushups - 5;
          lcd.print(pushups);
        }
      }
      if (toggleuser == 0) {
        if (readKey > 50 && readKey < 250) {
          delay(400);
          lcd.setCursor(10, 1);
           jpushups = jpushups +5;
           lcd.print(jpushups);
        } else if (readKey > 250 && readKey < 400 ) {
          delay(400);
          lcd.setCursor(10, 1);
          jpushups = jpushups -5;
          lcd.print(jpushups);
        }
      }
      if (readKey > 400 && readKey < 450) {
        delay(100);
        readKey = analogRead(0);
        button = evaluateButton(readKey);
        switch (button) {
          case 4:  // This case will execute if the "back" button is pressed
          button = 0;
          activeButton = 1;
          break;
         }
    } 
    if (readKey < 650 && readKey > 639){
      switch(toggleuser) {
        case 0:
        delay(350);
        blinkev();
        toggleuser = 1;
        break;
        case 1:
        delay(350);
        blinkj();
        toggleuser = 0;
        break;
        };
     };
  } 
}

void menuItem4() { // Function executes when you select the 4th item from main menu
 int activeButton = 0;

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("10k Steps-a-Day:");
  lcd.setCursor(0,1);
  lcd.print("E:");
  lcd.print(steps);
  
  lcd.setCursor(8,1);
  lcd.print("J:");
  lcd.print(jsteps);
  int toggleuser = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (toggleuser == 1) {
      if (readKey > 50 && readKey < 250) {
        delay(400);
        lcd.setCursor(2, 1);
        steps = steps + 250;
        lcd.print(steps);
        } else if (readKey > 250 && readKey < 400 ) {
          delay(400);
          lcd.setCursor(2, 1);
          steps = steps - 250;
          lcd.print(steps);
        }
      }
      if (toggleuser == 0) {
        if (readKey > 50 && readKey < 250) {
          delay(400);
          lcd.setCursor(10, 1);
           jsteps = jsteps + 250;
           lcd.print(jpushups);
        } else if (readKey > 250 && readKey < 400 ) {
          delay(400);
          lcd.setCursor(10, 1);
          jsteps = jsteps - 250;
          lcd.print(jsteps);
        }
      }
      if (readKey > 400 && readKey < 450) {
        delay(100);
        readKey = analogRead(0);
        button = evaluateButton(readKey);
        switch (button) {
          case 4:  // This case will execute if the "back" button is pressed
          button = 0;
          activeButton = 1;
          break;
         }
    } 
    if (readKey < 650 && readKey > 639){
      switch(toggleuser) {
        case 0:
        delay(350);
        blinkev();
        toggleuser = 1;
        break;
        case 1:
        delay(350);
        blinkj();
        toggleuser = 0;
        break;
        };
     };
  } 
}

void menuItem5() { // Function executes when you select the 5th item from main menu
 int activeButton = 0;

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Run/HIIT:");
  lcd.setCursor(0,1);
  lcd.print("E:");
  lcd.print(workout);
  
  lcd.setCursor(8,1);
  lcd.print("J:");
  lcd.print(jworkout);
  int toggleuser = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (toggleuser == 1) {
      if (readKey > 50 && readKey < 250) {
        delay(400);
        lcd.setCursor(2, 1);
        workout = workout + 1;
        lcd.print(workout);
        } else if (readKey > 250 && readKey < 400 ) {
          delay(400);
          lcd.setCursor(2, 1);
          workout = workout - 1;
          lcd.print(workout);
        }
      }
      if (toggleuser == 0) {
        if (readKey > 50 && readKey < 250) {
          delay(400);
          lcd.setCursor(10, 1);
           jworkout = jworkout + 1;
           lcd.print(jworkout);
        } else if (readKey > 250 && readKey < 400 ) {
          delay(400);
          lcd.setCursor(10, 1);
          jworkout = jworkout-1 ;
          lcd.print(jworkout);
        }
      }
      if (readKey > 400 && readKey < 450) {
        delay(100);
        readKey = analogRead(0);
        button = evaluateButton(readKey);
        switch (button) {
          case 4:  // This case will execute if the "back" button is pressed
          button = 0;
          activeButton = 1;
          break;
         }
    } 
    if (readKey < 650 && readKey > 639){
      switch(toggleuser) {
        case 0:
        delay(350);
        blinkev();
        toggleuser = 1;
        break;
        case 1:
        delay(350);
        blinkj();
        toggleuser = 0;
        break;
        };
     };
  }
}

