#define ROWS 16
#define COLS 16

int arr[ROWS][COLS] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

#define RAS 4
#define CAS 5
#define D 6  // chip data IN
#define Q 8  // chip data OUT
#define WRITE 9
#define Q_LED 7
#define CLK_LED 12

unsigned long lastRefreshTime = 0;
const unsigned long refreshInterval = 2000;  // Refresh interval

void setup() {
  Serial.begin(9600);

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  pinMode(D, OUTPUT);
  pinMode(Q, INPUT);

  pinMode(RAS, OUTPUT);
  pinMode(CAS, OUTPUT);
  pinMode(WRITE, OUTPUT);

  pinMode(Q_LED, OUTPUT);
  pinMode(CLK_LED, OUTPUT);

  Serial.println("Type 'W row data', 'R row', 'RS row', 'ARR', 'RAM', or 'CPY' to interact.");
}

void loop() {
  if (millis() - lastRefreshTime >= refreshInterval) {
    refreshRAM();
    lastRefreshTime = millis();
  }

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("W ")) {
      int spaceIndex = command.indexOf(' ');
      if (spaceIndex != -1) {
        int row = command.substring(spaceIndex + 1, spaceIndex + 3).toInt();
        String data = command.substring(spaceIndex + 4);

        if (row >= 0 && row < ROWS && data.length() == COLS) {
          int dataBits[COLS];
          for (int i = 0; i < COLS; i++) {
            dataBits[i] = data[i] == '1' ? 1 : 0;
          }
          writeBits(row, dataBits);
          Serial.print("Data written to RAM row ");
          Serial.println(row);
        } else {
          Serial.println("Invalid input. Format: W row data (data must be 16 bits long).");
        }
      }
    } else if (command.startsWith("R ")) {
      int row = command.substring(2).toInt();
      if (row >= 0 && row < ROWS) {
        Serial.print("RAM Row ");
        Serial.print(row);
        Serial.print(": ");
        readBitsDirect(row);
        Serial.println();
      } else {
        Serial.println("Invalid row index. Must be between 0 and 15.");
      }
    } else if (command.startsWith("RS ")) {
      int row = command.substring(3).toInt();
      if (row >= 0 && row < ROWS) {
        Serial.print("RAM Row ");
        Serial.print(row);
        Serial.print(": ");
        readBitsDirectSlowly(row);
        Serial.println();
      } else {
        Serial.println("Invalid row index. Must be between 0 and 15.");
      }
    } else if (command.equals("ARR")) {
      Serial.println("Array contents:");
      for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
          Serial.print(arr[row][col]);
        }
        Serial.println();
      }
    } else if (command.equals("RAM")) {
      Serial.println("Contents of RAM:");
      for (int row = 0; row < ROWS; row++) {
        Serial.print(row);
        Serial.print(": ");
        readBitsDirect(row);
        Serial.println();
      }
    } else if (command.equals("CPY")) {
      copyArrayToRAM();
      Serial.println("Array copied to RAM.");
    } else {
      Serial.println("Invalid command. Use 'W row data', 'R row', 'ARR', 'RAM', or 'CPY'.");
    }
  }
}

void readBitsDirect(int row) {
  digitalWrite(RAS, HIGH);
  digitalWrite(CAS, HIGH);
  digitalWrite(WRITE, HIGH);
  for (int i = 0; i < COLS; i++) {
    setRowAddress(row);
    digitalWrite(RAS, LOW);
    setColumnAddress(i);
    digitalWrite(CAS, LOW);
    Serial.print(digitalRead(Q));
    digitalWrite(RAS, HIGH);
    digitalWrite(CAS, HIGH);
  }
}

void readBitsDirectSlowly(int row) {
  digitalWrite(RAS, HIGH);
  digitalWrite(CAS, HIGH);
  digitalWrite(WRITE, HIGH);
  for (int i = 0; i < COLS; i++) {
    refreshRAM();
    digitalWrite(CLK_LED, LOW);
    setRowAddress(row);
    digitalWrite(RAS, LOW);
    setColumnAddress(i);
    digitalWrite(CAS, LOW);
    digitalWrite(Q_LED, digitalRead(Q));
    Serial.print(digitalRead(Q));
    delay(100);
    digitalWrite(RAS, HIGH);
    digitalWrite(CAS, HIGH);
    digitalWrite(CLK_LED, HIGH);
    delay(100);
    refreshRAM();
  }
}

void writeBits(int row, int data[COLS]) {
  digitalWrite(RAS, HIGH);
  digitalWrite(CAS, HIGH);
  for (int i = 0; i < COLS; i++) {
    setRowAddress(row);
    digitalWrite(RAS, LOW);
    digitalWrite(WRITE, LOW);
    digitalWrite(D, data[i]);
    setColumnAddress(i);
    digitalWrite(CAS, LOW);
    digitalWrite(RAS, HIGH);
    digitalWrite(CAS, HIGH);
    digitalWrite(WRITE, HIGH);
  }
}

void setRowAddress(int row) {
  digitalWrite(A0, bitRead(row, 0));
  digitalWrite(A1, bitRead(row, 1));
  digitalWrite(A2, bitRead(row, 2));
  digitalWrite(A3, bitRead(row, 3));
  digitalWrite(A4, bitRead(row, 4));
  digitalWrite(A5, bitRead(row, 5));
  digitalWrite(10, bitRead(row, 6));
  digitalWrite(11, bitRead(row, 7));
}

void setColumnAddress(int col) {
  digitalWrite(A0, bitRead(col, 0));
  digitalWrite(A1, bitRead(col, 1));
  digitalWrite(A2, bitRead(col, 2));
  digitalWrite(A3, bitRead(col, 3));
  digitalWrite(A4, bitRead(col, 4));
  digitalWrite(A5, bitRead(col, 5));
  digitalWrite(10, bitRead(col, 6));
  digitalWrite(11, bitRead(col, 7));
}

void copyArrayToRAM() {
  for (int row = 0; row < ROWS; row++) {
    writeBits(row, arr[row]);
  }
}

void refreshRAM() {
  for (int row = 0; row < ROWS; row++) {
    setRowAddress(row);
    digitalWrite(RAS, LOW);
    delayMicroseconds(1); // Small delay to mimic the access time
    digitalWrite(RAS, HIGH);
  }
}
