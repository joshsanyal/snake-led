const byte ANODE_PINS[8] = {2, 3, 4, 5, A0, A1, A2, A3};
const byte CATHODE_PINS[8] = {6, 7, 8, 9, 10, 11, 12, 13};
const byte JOYS_VRX = A4;
const byte JOYS_VRY = A5;
const byte JOYS_SW = 1;
const int delay_time = 500; // Number of ms in between display updates

byte SNAKE[64][2]; // Set of (x, y) coords (head of snake at the end)
byte SNAKE_LEN; // Tracks length of the snake
byte DIR; // Direction of snake: 0 = right, 1 = left, 2 = up, 3 = down
byte ALIVE; // Tracks if the snake is still alive

byte FRUIT[2]; // (x, y) of the fruit
byte BOARD[8][8]; // Tracks the board state
long int reset_time; // Tracks when to update the LED display

// CLEARS THE BOARD STATE
void clearBoard() {
  for (byte x = 0; x < 8; x++) {
    for (byte y = 0; y < 8; y++) {
      BOARD[x][y] = 0;
    }
  }
}

// UPDATES THE BOARD WITH THE SNAKE
void updateBoard() {
  clearBoard();
  for (byte i = 0; i < SNAKE_LEN; i++) {
    BOARD[SNAKE[i][0]][SNAKE[i][1]] = 1;
  }
}

// GENERATES A NEW RANDOM FRUIT
void genFruit() {
  byte x = random(0, 8);
  byte y = random(0, 8);
  while (BOARD[x][y] == 1) {
    x = random(0, 8);
    y = random(0, 8);
  }
  FRUIT[0] = x;
  FRUIT[1] = y;
  BOARD[x][y] = 1;
}

// INTIALIZES THE GAME START STATE
void startGame() {
  SNAKE_LEN = 3;
  DIR = 0;
  ALIVE = 1;
  for (byte x = 7; x > 7 - SNAKE_LEN; x--) {
    SNAKE[7 - x][0] = x;
    SNAKE[7 - x][1] = 4;
  }
  updateBoard();
  genFruit();
  reset_time = millis() + delay_time;
}

// CONTROLS THE SNAKE MOVING
void move() {
  // FIND WHERE THE SNAKE IS MOVING
  byte x_new = SNAKE[SNAKE_LEN - 1][0];
  byte y_new = SNAKE[SNAKE_LEN - 1][1];
  if (DIR == 0) {
    x_new--;
  } else if (DIR == 1) {
    x_new++;
  } else if (DIR == 2) {
    y_new--;
  } else {
    y_new++;
  }

  // CHECK IF MOVING OUT OF BOUNDS
  if (x_new < 0 || x_new > 7 || y_new < 0 || y_new > 7) {
    ALIVE = 0;
    return;
  }
  // CHECK IF EATS THE FRUIT
  if (FRUIT[0] == x_new && FRUIT[1] == y_new) {
    SNAKE[SNAKE_LEN][0] = x_new;
    SNAKE[SNAKE_LEN][1] = y_new;
    SNAKE_LEN++;
    updateBoard();
    genFruit();
    return;
  }
  // CHECK FOR COLLISION WITH ITSELF
  if (BOARD[x_new][y_new] == 1 && !(SNAKE[0][0] == x_new && SNAKE[0][1] == y_new)) {
    ALIVE = 0;
    return;
  }
  // MOVE THE SNAKE NORMALLY
  for (byte i = 0; i < SNAKE_LEN - 1; i++) {
    SNAKE[i][0] = SNAKE[i+1][0];
    SNAKE[i][1] = SNAKE[i+1][1];
  }
  SNAKE[SNAKE_LEN-1][0] = x_new;
  SNAKE[SNAKE_LEN-1][1] = y_new;
  updateBoard();
  BOARD[FRUIT[0]][FRUIT[1]] = 1;
}

// DISPLAYS THE CURRENT BOARD STATE
void display() {
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      if (BOARD[i][j] == 0) {
        digitalWrite(CATHODE_PINS[j], HIGH);
      } else {
        digitalWrite(CATHODE_PINS[j], LOW);
      }
    }
    digitalWrite(ANODE_PINS[i], LOW);
    delay(2);
    digitalWrite(ANODE_PINS[i], HIGH);
  }
}

// READS THE JOYSTICK INPUT TO CHANGE DIRECTION
void readInput() {
  int X_STICK = analogRead(JOYS_VRX);
  int Y_STICK = analogRead(JOYS_VRY);
  if (X_STICK > 1000 && (DIR == 2 || DIR == 3)) {
    DIR = 0;
    reset_time = millis() + delay_time;
    move();
  } else if (X_STICK < 50 && (DIR == 2 || DIR == 3)) {
    DIR = 1;
    reset_time = millis() + delay_time;
    move();
  } else if (Y_STICK < 50 && (DIR == 0 || DIR == 1)) {
    DIR = 2;
    reset_time = millis() + delay_time;
    move();
  } else if (Y_STICK > 1000 && (DIR == 0 || DIR == 1)) {
    DIR = 3;
    reset_time = millis() + delay_time;
    move();
  }
}

void setup() {
  for (byte i = 0; i < 8; i++) {
    pinMode(ANODE_PINS[i], OUTPUT);
    pinMode(CATHODE_PINS[i], OUTPUT);
  }
  for (byte i = 0; i < 8; i++) {
    digitalWrite(ANODE_PINS[i], HIGH);
    digitalWrite(CATHODE_PINS[i], HIGH);
  }
  pinMode(JOYS_SW, INPUT);
  digitalWrite(JOYS_SW, HIGH); 
  startGame();
}

void loop() {
  display();
  if (ALIVE == 1) {
    if (millis() > reset_time) {
      reset_time = millis() + delay_time;
      move();
    }
    readInput();
  }
  else if (!digitalRead(JOYS_SW)) {
    startGame();  // restart the game if button pressed
  } 
  else {
    // flashing animation to indicate game is over
    if (millis() > reset_time) {
      reset_time = millis() + delay_time;
      if (BOARD[SNAKE[0][0]][SNAKE[0][1]] == 1) {
        clearBoard();
      } else {
        updateBoard();
      }
      BOARD[FRUIT[0]][FRUIT[1]] = 1;
    }
  }
}


