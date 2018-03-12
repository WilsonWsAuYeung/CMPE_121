/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
int SendFLAG = 0;
int WHOSTURN = 1; // 0 MEANS YOU ARE BLUE      1 MEANS YOU ARE RED
int Sequence = 0;
int RANDOMFLAG = 1;
int OPPONENTID = 1;
int usercount = 1;   // turncounter
int user = 1;        // modulo 2 usercount to see if blue turn or red turn
int flag = 0;        // every millisecond
int column = 0;      // checks which column to write in
int row = 0;         // checks which row to write in (1=8)(2=9)
int flag2 = 0;       // causes to blink mod 2
int colorcursor = 0; // modulo 2 usercount blue or red cursor
int up = 0;          // up counter for cursor
int down = 0;        // down counter for cursor
int left = 0;        // left counter for cursor
int right = 0;       // right counter for cursor
int vertical = 0;    // down-up
int horizontal = 0;  // right - left
int large = 0;       // checks if it is 16x16
int occupied = 0;    // checks if any marble is there
int occupied2 = 0;   // checks if red or blue marble is there
int mask;            // mask for upper half
int mask2;           // mask for lower half
int valid = 0;       // valid move
int validflag = 0;   // valid move return from ValidMove()
int Vmoves = 0;      // Vertical counter from [0][0] to [dimension][dimension]
int Hmoves = 0;      // Horizontal counter from [0][0] to [dimension][dimension]
int MoveCount =
    0; // if there are possible moves from [0][0] to [dimension][dimension]
int MoveFlag = 0; // Flag for above^
int Possible = 1; // possible moves
int Show = 0;     // show possible moves
int PossibleFlag = 1;
int DontShow = 0;
int ShowCounter = 0;
int RefreshShow = 0;
int MoveTot = 1;
int MoveTot2 = 0;
int TempHori =
    0; // Gets put in Valid Move to see if Opponent can place tile there
int TempVerti =
    0; // Gets put in Valid Move to see if Opponent can place tile there
int OPPONENTVALIDMOVEFLAG =
    0;            // If Valid Move is true during opponents turn its turned on
int ISKIPPED = 0; // Sees if I skipped
int OPPONENTSKIPPED = 0; // Sees if opponent skipps
int blockcountU = 0;     // Sees how many tiles to turn U
int blockcountD = 0;     // Sees how many tiles to turn D
int blockcountL = 0;     // Sees how many tiles to turn L
int blockcountR = 0;     // Sees how many tiles to turn R
int blockcountNW = 0;    // Sees how many tiles to turn NW
int blockcountNE = 0;    // Sees how many tiles to turn NE
int blockcountSW = 0;    // Sees how many tiles to turn SW
int blockcountSE = 0;    // Sees how many tiles to turn SE
int blockcountU2 = 0;    // Sees how many tiles to turn U
int blockcountD2 = 0;    // Sees how many tiles to turn D
int blockcountL2 = 0;    // Sees how many tiles to turn L
int blockcountR2 = 0;    // Sees how many tiles to turn R
int blockcountNW2 = 0;   // Sees how many tiles to turn NW
int blockcountNE2 = 0;   // Sees how many tiles to turn NE
int blockcountSW2 = 0;   // Sees how many tiles to turn SW
int blockcountSE2 = 0;   // Sees how many tiles to turn SE
int PassNotValid = 1;
int paste = 0;      // pastes the tiles to be turned over
int i = 0;          // for loop
int j = 0;          // for loop
int y = 0;          // packet for-loop
#define dimension 4 // 4x4 8x8 16x16
int PacketReceived = 0;
#define FILENAME "game.txt"
#include "FS.h"
#include "stdio.h"
#include <project.h>
#define ASCIIConversion 15 // CONVERTING ASCII to INTEGER VALUES MASK

#if defined(__GNUC__)
/* Add an explicit reference to the floating point printf library */
/* to allow usage of the floating point conversion specifiers. */
/* This is not linked in by default with the newlib-nano library. */
asm(".global _printf_float");
#endif

#define USBUART_BUFFER_dimension (64u)
#define LINE_STR_LENGTH (20u)
#define USBFS_DEVICE (0u)
// ProtoTyping7
int MainState();    // State Machine for Main
int ValidMove();    // checks if you are allowed to execute
int InData();       // Read in data from coolterm
int ValidUDLR();    // Valid Move for Up Down Left Right
int ValidCorners(); // Valid Move for NE NW SE SW
int MoveSets(int i, int j);
int WriteLED();      // Masking logic
int ReceivedData();  // Checking USBUART for inputs
int SendPacket();    // Send Packets every 500 milliseconds
int Avail();         // Available moves (7)
int EndConditions(); // Checks for End Conditions
int receivedude();   // State Machine for RX
int UPDATEPACKET();  // UPDATE PACKET when I send a move
int Debug();         // LCD and LED p6[2]
int WIFISTATE();     // Wifi State
int SDInit();
int ParsePacket();

CY_ISR(ISR) // Refresh every millisecond
{
  if (flag == 0) {
    flag = 1; // sets flag to run LED Function
  }
  Timer_ReadStatusRegister(); // refreshes timer
}
CY_ISR(ISR2) // blinks every half second
{
  flag2++;                      // counter modulo 2
  Timer_1_ReadStatusRegister(); // refreshes timer
}

CY_ISR(ISR3) // transmits every half second
{
  Control_Reg_1_Write(1);
  Timer_2_ReadStatusRegister(); // refreshes timer
}
CY_ISR(ISR4) { // Rx
  receivedude();
}

CY_ISR(ISR1) { SendPacket(); }
uint8 red[dimension][dimension];   // Sets red
uint8 green[dimension][dimension]; // Sets green
uint8 blue[dimension][dimension];  // Sets blue
uint8 buffer[1024];
uint16 count;
uint8 byte = 0; // EACH BYTE IN RX

uint8 message[19]; // TX Update packet
int sendIndex = 0;

uint8 receiveMessage[19]; // RX packet
int messageIndex = 0;

uint8 WifiMessage[15]; // RX packet for Wifi
int wifiIndex = 0;

int state = 0;    // STATE VALUE for RX STATE MACHINE
int WifiGame = 1; // STATE VALUE FOR MAIN STATE MACHINE
int WifiAdv = 1;  // STATE VALUE FOR WIFI STATE MACHINE

FS_FILE *pFile;
char sdVolName[10];
char sdFile[9] = "File.txt";

uint8 Status_Rx = 0;
char *WifiAdvertise = "ADVERTISE wilson00\n";
char *WifiConnect = "CONNECT 192.168.0.109\n";
int WifiAdvertiseIndex = 0;

char TheFULLString[25] = {'d',  'a',  't',  'a',  ' ',  0x55, 0xaa, 'w',  'i',
                          'l',  's',  'o',  'n',  '0',  '0',  0x20, 0x30, 0x30,
                          0x31, 0x30, 0x30, 0x31, 0x30, 0x32, '\n'};

char WILSON[8] = "wilson00";
char Opponent[8] = "00000000";
char SD_Moves[14];
int main() {
  UART_Start();
#if (CY_PSOC3 || CY_PSOC5LP)
#endif /* (CY_PSOC3 || CY_PSOC5LP) */
  CyGlobalIntEnable;
  if (dimension == 16) { // if 16x16 use R2G2B2
    large = 1;
  }
  blue[(dimension / 2) - 1][(dimension / 2) - 1] = 1; // sets initial tiles
  blue[(dimension / 2)][(dimension / 2)] = 1;
  red[(dimension / 2) - 1][(dimension / 2)] = 1;
  red[(dimension / 2)][(dimension / 2) - 1] = 1;

  CyGlobalIntEnable;  /* Enable global interrupts. */
  isr_StartEx(ISR);   // ISR for Refresh
  isr2_StartEx(ISR2); // ISR for every half second
  isr3_StartEx(ISR3); // ISR send every half second for transmit
  isr4_StartEx(ISR4); // RX receive
  isr_1_StartEx(ISR1);
  LCD_Start();
  Timer_Start();   // Timer for Refresh
  Timer_1_Start(); // Timer for every half-second
  Timer_2_Start(); // Send every half second
  USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);

  FS_Init();
  FS_GetVolumeName(0u, &sdVolName[0], 9u);
  FS_FormatSD(sdVolName);
  FS_MkDir("Dir");
  pFile = FS_FOpen(sdFile, "w");
  FS_Write(pFile, "0123456789", 10u);
  FS_FClose(pFile);
  FS_CopyFile(sdFile, "\\Dir\\file1.txt");

  message[0] = 0x55; // starting bit
  message[1] = 0xaa; // starting bit
  for (y = 2; y <= 9; y++) {
    message[y] = (uint8)WILSON[y - 2]; // My USER ID
  }
  message[10] = 0x20;                    // End of User ID
  message[11] = 0x30 + (Sequence / 100); // MSB of Sequence
  message[12] = 0x30 + ((Sequence % 100) / 10);
  message[13] = 0x30 + (Sequence % 10);
  message[14] = 0x30; // Skip
  message[15] = 0x30; // row
  message[16] = 0x31;
  message[17] = 0x30; // column
  message[18] = 0x31;
  for (;;) {
    // UPDATEPACKET();
    MainState();

    // PacketReceived=0;
    // LCD_Position(0u, 0u);
    // LCD_PrintNumber(Sequence);
  }
}
int SDInit() { // initializing SD card

  return 0;
}
int MainState() { // State machine for my main function
  switch (WifiGame) {
  case 0:
    WIFISTATE();
    break;
  case 1:
    InData();
    ReceivedData();
    WriteLED();
    Avail();
    EndConditions();
    break;
  case 2: // Game Over

    // WriteLED();
    break;
  }
  return 0;
}
int WIFISTATE() { // state machine to set up wifi connection
  switch (WifiAdv) {
  case 0: // Setup
    break;
  case 1: // Advertisement
    if (WifiAdvertiseIndex < 18) {
      // UART_WriteTxData(WifiAdvertise[WifiAdvertiseIndex]);
      WifiAdvertiseIndex++;
    } else {
      break;
    }
  case 2: // connect to player
    break;
  case 3: // Connected
    break;
  case 4: // Who Starts First
    break;
  }
  return 0;
}
int UPDATEPACKET() {
  message[0] = 0x55;
  message[1] = 0xaa;
  for (y = 2; y <= 9; y++) {
    message[y] = (uint8)WILSON[y - 2];
  }
  message[10] = 0x20;
  message[11] = 0x30 + (Sequence / 100);
  message[12] = 0x30 + ((Sequence % 100) / 10);
  message[13] = 0x30 + (Sequence % 10);
  message[14] = 0x30 + ISKIPPED;              // pass flag
  message[15] = 0x30 + ((vertical + 1) / 10); // row
  message[16] = 0x30 + ((vertical + 1) % 10);
  message[17] = 0x30 + ((horizontal + 1) / 10); // column
  message[18] = 0x30 + ((horizontal + 1) % 10);
  return 0;
}
int ParsePacket() {

  message[11] = TheFULLString[16]; // updating my packet before sending through
                                   // wifi connection
  message[12] = TheFULLString[17];
  message[13] = TheFULLString[18];
  message[14] = TheFULLString[19];
  message[15] = TheFULLString[20];
  message[16] = TheFULLString[21];
  message[17] = TheFULLString[22];
  message[18] = TheFULLString[23];

  return 0;
}
int receivedude() {
  switch (state) {
  case 0: // receive all incoming data regardless of packets
    Status_Rx = UART_ReadRxStatus();
    LED_Write(1);
    while (Status_Rx & UART_RX_STS_FIFO_NOTEMPTY) {
      byte = UART_ReadRxData();
      // USBUART_PutChar(byte);
      WifiMessage[wifiIndex] = byte;
      wifiIndex++;
      if (wifiIndex == 19) {
        // while (0u == USBUART_CDCIsReady())
        //{
        //}
        USBUART_PutData(WifiMessage, 19);
        // USBUART_PutData(NULL, 0u);
        wifiIndex = 0;
      }
      Status_Rx = UART_ReadRxStatus();
    }
    // LCD_Position(0,0);
    // naw++;
    // LCD_PrintNumber(naw);
    break;
  case 1:
    //            if(UART_ReadRxStatus() & UART_RX_STS_FIFO_NOTEMPTY){ //
    //            receiving correct packets
    byte = UART_ReadRxData();
    receiveMessage[messageIndex] = byte;
    switch (messageIndex) {
    case 0:
      if (byte == 0x55) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 1:
      if (byte == 0xaa) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 2:
      if (OPPONENTID) {
        Opponent[0] = byte;
        messageIndex++;
      } else if (byte == Opponent[0]) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 3:
      if (OPPONENTID) {
        Opponent[1] = byte;
        messageIndex++;
      } else if (byte == Opponent[1]) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 4:
      if (OPPONENTID) {
        Opponent[2] = byte;
        messageIndex++;
      } else if (byte == Opponent[2]) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 5:
      if (OPPONENTID) {
        Opponent[3] = byte;
        messageIndex++;
      } else if (byte == Opponent[3]) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 6:
      if (OPPONENTID) {
        Opponent[4] = byte;
        messageIndex++;
      } else if (byte == Opponent[4]) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 7:
      if (OPPONENTID) {
        Opponent[5] = byte;
        messageIndex++;
      } else if (byte == Opponent[5]) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 8:
      if (OPPONENTID) {
        Opponent[6] = byte;
        messageIndex++;
      } else if (byte == Opponent[6]) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 9:
      if (OPPONENTID) {
        Opponent[7] = byte;
        messageIndex++;
      } else if (byte == Opponent[7]) {
        messageIndex++;
        // LED_Write(1);
      } else {
        messageIndex = 0;
      }
      break;
    case 10:
      if (byte == 0x20) {
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 11:
      if (WHOSTURN) {
        if (byte == 0x30 + (Sequence / 100)) {
          messageIndex++;
        } else {
          messageIndex = 0;
        }
      }
      if (!WHOSTURN) {
        if (byte == 0x30 + ((Sequence + 1) / 100)) {
          messageIndex++;
        } else {
          messageIndex = 0;
        }
      }
      break;
    case 12:
      if (WHOSTURN) {
        if (byte == 0x30 + ((Sequence % 100) / 10)) {
          messageIndex++;
        } else {
          messageIndex = 0;
        }
      }
      if (!WHOSTURN) {
        if (byte == 0x30 + (((Sequence + 1 % 100)) / 10)) {
          messageIndex++;
        } else {
          messageIndex = 0;
        }
      }
      break;
    case 13:
      if (WHOSTURN) {
        if (byte == 0x30 + (Sequence % 10)) {
          messageIndex++;
        } else {
          messageIndex = 0;
        }
      }
      if (!WHOSTURN) { // blue
        if (byte == 0x30 + ((Sequence + 1) % 10)) {
          messageIndex++;
        } else {
          messageIndex = 0;
        }
      }
      break;
    case 14:
      if (byte == 0x30) {
        messageIndex++;
      } else if (byte == 0x31) {
        OPPONENTSKIPPED = 1;
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 15:
      messageIndex++; // MSB ROW
      break;
    case 16:
      if (((receiveMessage[15] & ASCIIConversion) * 10 + receiveMessage[16] -
               49 <=
           16) ||
          OPPONENTSKIPPED) { // LSB ROW ||/*OPPONENTSKIPPED*/
        messageIndex++;
      } else {
        messageIndex = 0;
      }
      break;
    case 17:
      messageIndex++; // MSG COLUMN
      break;
    case 18:
      if (((receiveMessage[17] & ASCIIConversion) * 10 + receiveMessage[18] -
               49 <=
           16) ||
          OPPONENTSKIPPED) { //
        USBUART_PutData(receiveMessage, 19);
        OPPONENTID = 0;
        if (!RANDOMFLAG) {
          OPPONENTSKIPPED = 0;
        }
        if (OPPONENTSKIPPED && RANDOMFLAG) {
          messageIndex = 0;
          usercount++;
          user = usercount % 2;
          RANDOMFLAG = 0;
          OPPONENTSKIPPED = 0;
          if (!WHOSTURN) {
            // Sequence++;
          }
          PassNotValid = 0;
          break;
        }
        OPPONENTSKIPPED = 0;
        if (!OPPONENTSKIPPED && (PassNotValid == 1)) {
          messageIndex = 0;
          TempVerti = (receiveMessage[15] & ASCIIConversion) * 10 +
                      receiveMessage[16] - 49;
          TempHori = (receiveMessage[17] & ASCIIConversion) * 10 +
                     receiveMessage[18] - 49;

          OPPONENTVALIDMOVEFLAG = MoveSets(TempVerti, TempHori);
          if (OPPONENTVALIDMOVEFLAG) {
            vertical = (receiveMessage[15] & ASCIIConversion) * 10 +
                       receiveMessage[16] - 49;
            horizontal = (receiveMessage[17] & ASCIIConversion) * 10 +
                         receiveMessage[18] - 49;
            RANDOMFLAG = 1;
            sprintf(SD_Moves, "%s %d %d %d", Opponent, 0, horizontal, vertical);

            if (!WHOSTURN) {
              // Sequence++;
            }
            ISKIPPED = 0;
          }
          up = 0;
          down = vertical;
          left = 0;
          right = horizontal;
          valid = ValidMove();

        } else {
        }
      } else {
        messageIndex = 0;
        break;
      }
      break;

    default:
      messageIndex = 0;
      break;
    }
  }
  //    }
  return 0;
}
int SendPacket() { // send packet through Tx in the UART

  if (SendFLAG) {
    UART_PutString(TheFULLString);
  }
  // UART_WriteTxData(message[sendIndex]);
  sendIndex++;
  if (sendIndex >= 19) {
    sendIndex = 0;
    Control_Reg_1_Write(0);
  }

  return 0;
}

int InData() {
  if (0u != USBUART_IsConfigurationChanged()) {
    /* Initialize IN endpoints when device is configured. */
    if (0u != USBUART_GetConfiguration()) {
      /* Enumeration is done, enable OUT endpoint to receive data
       * from host. */
      USBUART_CDC_Init();
    }
  }

  /* Service USB CDC when device is configured. */
  if ((0u != USBUART_GetConfiguration())) //&&user==WHOSTURN)
  {
    /* Check for input data from host. */
    if (0u != USBUART_DataIsReady()) {
      /* Read received data and re-enable OUT endpoint. */
      count = USBUART_GetAll(buffer);

      if (0u != count) {
        /* Wait until component is ready to send data to host. */
        while (0u == USBUART_CDCIsReady()) {
        }
        /* Send data back to host. */
        USBUART_PutData(buffer, count);

        /* If the last sent packet is exactly the maximum packet
         *  dimension, it is followed by a zero-length packet to assure
         *  that the end of the segment is properly identified by
         *  the terminal.
         */
        if (USBUART_BUFFER_dimension == count) {
          /* Wait until component is ready to send data to PC. */
          while (0u == USBUART_CDCIsReady()) {
          }

          /* Send zero-length packet to PC. */
          USBUART_PutData(NULL, 0u);
        }
        if ((buffer[0] == 31) && (vertical < (dimension - 1))) // 2 --31 --50
        {
          down++;
        }
        if ((buffer[0] == 29) && (horizontal < dimension - 1)) // 6 --29 --54
        {
          right++;
        }
        if ((buffer[0] == 30) && (vertical > 0)) // 8 --30 -- 56
        {
          up++;
        }
        if ((buffer[0] == 28) && (horizontal > 0)) // 2 --28 --52
        {
          left++;
        }
        if (buffer[0] == 'a') // 2 --28 --52
        {
          UART_PutString(WifiAdvertise);
        }
        if (buffer[0] == 'c') // 2 --28 --52
        {
          UART_PutString(WifiConnect);
          //                        UART_ClearRxBuffer();
          //                        UART_ClearTxBuffer();
          state = 1;
        }

        if ((buffer[0] == 1) && (horizontal > 0)) // 9 --115 --57
        {
          right = 0;
          left = 0;
          up = 0;
          down = 0;
        }
        if (buffer[0] == 83) {
          // usercount++;
          PossibleFlag = 1;
          ISKIPPED = 1;
          Sequence++;
          usercount++;
          user = usercount % 2;
          UPDATEPACKET();

          if (ShowCounter % 2 == 1) {
            RefreshShow = 1;
          }
        }
        if (buffer[0] == 55) { // 7
          ShowCounter++;
          if (ShowCounter % 2 == 1) {
            Show = 1;
          }
          if (ShowCounter % 2 == 0) {
            DontShow = 1;
          }
        }

        horizontal = right - left;
        vertical = down - up;
        user = usercount % 2;
        valid = ValidMove();

        if ((buffer[0] == 13) && valid >= 1) // 5 --53
        {
          OPPONENTSKIPPED = 0;
          Sequence++;
          PassNotValid = 1;
          // RANDOMFLAG=1;
          ParsePacket();
          UPDATEPACKET();
          SendFLAG = 1;
          sprintf(SD_Moves, "%s %d %d %d", WILSON, 0, horizontal, vertical);
          FS_Write(pFile, SD_Moves, 14u);
          PossibleFlag = 1;
        }
      }
    }
  }
  return 0;
}
int ReceivedData() {
  // if((buffer[0] == 13)&&valid>=1&&(user==0))
  if ((((buffer[0] == 13) && valid >= 1 && (user == 0)) && (!WHOSTURN)) ||
      (OPPONENTVALIDMOVEFLAG &&
       user == 0)) //&&(!WHOSTURN)))//||(PacketReceived&&user==0)) //5 --53
  {
    blue[vertical][horizontal] = 1;
    for (paste = 0; paste < blockcountU; paste++) {
      blue[vertical - 1 - paste][horizontal] = 1;
      red[vertical - 1 - paste][horizontal] = 0;
    }
    for (paste = 0; paste < blockcountD; paste++) {
      blue[vertical + 1 + paste][horizontal] = 1;
      red[vertical + 1 + paste][horizontal] = 0;
    }
    for (paste = 0; paste < blockcountL; paste++) {
      blue[vertical][horizontal - 1 - paste] = 1;
      red[vertical][horizontal - 1 - paste] = 0;
    }
    for (paste = 0; paste < blockcountR; paste++) {
      blue[vertical][horizontal + 1 + paste] = 1;
      red[vertical][horizontal + 1 + paste] = 0;
    }
    for (paste = 0; paste < blockcountNW; paste++) {
      blue[vertical - 1 - paste][horizontal - 1 - paste] = 1;
      red[vertical - 1 - paste][horizontal - 1 - paste] = 0;
    }
    for (paste = 0; paste < blockcountNE; paste++) {
      blue[vertical - 1 - paste][horizontal + 1 + paste] = 1;
      red[vertical - 1 - paste][horizontal + 1 + paste] = 0;
    }
    for (paste = 0; paste < blockcountSW; paste++) {
      blue[vertical + 1 + paste][horizontal - 1 - paste] = 1;
      red[vertical + 1 + paste][horizontal - 1 - paste] = 0;
    }
    for (paste = 0; paste < blockcountSE; paste++) {
      blue[vertical + 1 + paste][horizontal + 1 + paste] = 1;
      red[vertical + 1 + paste][horizontal + 1 + paste] = 0;
    }
    if (ShowCounter % 2 == 1) {
      RefreshShow = 1;
    }
    usercount++;
    user = usercount % 2;
    valid = 0;
    PacketReceived = 0;
    OPPONENTVALIDMOVEFLAG = 0;
    //            if(!WHOSTURN){
    //                        Sequence++;
    //            }
    UPDATEPACKET();
  }
  // if((buffer[0] == 13)&&valid>=1&&(user==1))
  if ((((buffer[0] == 13) && valid >= 1 && (user == 1)) && (WHOSTURN)) ||
      (OPPONENTVALIDMOVEFLAG &&
       user == 1)) //&&(WHOSTURN)))//||(PacketReceived&&user==1))) //5
  {
    red[vertical][horizontal] = 1;
    for (paste = 0; paste < blockcountU; paste++) {
      red[vertical - 1 - paste][horizontal] = 1;
      blue[vertical - 1 - paste][horizontal] = 0;
    }
    for (paste = 0; paste < blockcountD; paste++) {
      red[vertical + 1 + paste][horizontal] = 1;
      blue[vertical + 1 + paste][horizontal] = 0;
    }
    for (paste = 0; paste < blockcountL; paste++) {
      red[vertical][horizontal - 1 - paste] = 1;
      blue[vertical][horizontal - 1 - paste] = 0;
    }
    for (paste = 0; paste < blockcountR; paste++) {
      red[vertical][horizontal + 1 + paste] = 1;
      blue[vertical][horizontal + 1 + paste] = 0;
    }
    for (paste = 0; paste < blockcountNW; paste++) {
      red[vertical - 1 - paste][horizontal - 1 - paste] = 1;
      blue[vertical - 1 - paste][horizontal - 1 - paste] = 0;
    }
    for (paste = 0; paste < blockcountNE; paste++) {
      red[vertical - 1 - paste][horizontal + 1 + paste] = 1;
      blue[vertical - 1 - paste][horizontal + 1 + paste] = 0;
    }
    for (paste = 0; paste < blockcountSW; paste++) {
      red[vertical + 1 + paste][horizontal - 1 - paste] = 1;
      blue[vertical + 1 + paste][horizontal - 1 - paste] = 0;
    }
    for (paste = 0; paste < blockcountSE; paste++) {
      red[vertical + 1 + paste][horizontal + 1 + paste] = 1;
      blue[vertical + 1 + paste][horizontal + 1 + paste] = 0;
    }
    if (ShowCounter % 2 == 1) {
      RefreshShow = 1;
    }
    usercount++;
    user = usercount % 2;
    valid = 0;
    PacketReceived = 0;
    OPPONENTVALIDMOVEFLAG = 0;

    //                    if(WHOSTURN){
    //                        Sequence++;
    //                    }
    UPDATEPACKET();
  }
  return 0;
}
int ValidMove() { // checks for valid move
  occupied = 0;
  blockcountU = 0;
  blockcountD = 0;
  blockcountL = 0;
  blockcountR = 0;
  blockcountNW = 0;
  blockcountNE = 0;
  blockcountSW = 0;
  blockcountSE = 0;
  validflag = 0;
  if ((red[vertical][horizontal] == 1) || (blue[vertical][horizontal] == 1)) {
    occupied = 1;
  }
  ValidUDLR();
  ValidCorners();
  return validflag;
}

int ValidUDLR() {                     // checks for valid move
  if ((occupied == 0) && user == 0) { // blue turn
    if (vertical > 1) {
      while (red[vertical - 1 - blockcountU][horizontal]) { // up
        blockcountU++;
        if (vertical - 1 - blockcountU < 0) {
          blockcountU = 0;
          break;
        }
        if (blue[vertical - 1 - blockcountU][horizontal]) {
          validflag++;
          break;
        }

        if (red[vertical - 1 - blockcountU][horizontal] == 0) {
          blockcountU = 0;
          break;
        }
      }
    }
    if (vertical < dimension - 2) {
      while (red[vertical + 1 + blockcountD][horizontal]) { // down
        blockcountD++;
        if (vertical + 1 + blockcountD > dimension - 1) {
          blockcountD = 0;
          break;
        }
        if (blue[vertical + 1 + blockcountD][horizontal]) {
          validflag++;
          break;
        }
        if (red[vertical + 1 + blockcountD][horizontal] == 0) {
          blockcountD = 0;
          break;
        }
      }
    }
    if (horizontal > 1) {
      while (red[vertical][horizontal - blockcountL - 1]) { // left
        blockcountL++;
        if (horizontal - 1 - blockcountL < 0) {
          blockcountL = 0;
          break;
        }
        if (blue[vertical][horizontal - blockcountL - 1]) {
          validflag++;
          break;
        }
        if (red[vertical][horizontal - blockcountL - 1] == 0) {
          blockcountL = 0;
          break;
        }
      }
    }
    if (horizontal < dimension - 2) {
      while (red[vertical][horizontal + blockcountR + 1]) { // right
        blockcountR++;
        if (horizontal + 1 + blockcountR > dimension - 1) {
          blockcountR = 0;
          break;
        }
        if (blue[vertical][horizontal + blockcountR + 1]) {
          validflag++;
          break;
        }
        if (red[vertical][horizontal + blockcountR + 1] == 0) {
          blockcountR = 0;
          break;
        }
      }
    }
  }
  if ((occupied == 0) && user == 1) { // red turn
    if (vertical > 1) {
      while (blue[vertical - 1 - blockcountU][horizontal]) { // up
        blockcountU++;
        if (vertical - 1 - blockcountU < 0) {
          blockcountU = 0;
          break;
        }
        if (red[vertical - 1 - blockcountU][horizontal]) {
          validflag++;
          break;
        }
        if (blue[vertical - 1 - blockcountU][horizontal] == 0) {
          blockcountU = 0;
          break;
        }
      }
    }
    if (vertical < dimension - 2) {
      while (blue[vertical + 1 + blockcountD][horizontal]) { // down
        blockcountD++;
        if (vertical + 1 + blockcountD > dimension - 1) {
          blockcountD = 0;
          break;
        }
        if (red[vertical + 1 + blockcountD][horizontal]) {
          validflag++;
          break;
        }
        if (blue[vertical + 1 + blockcountD][horizontal] == 0) {
          blockcountD = 0;
          break;
        }
      }
    }
    if (horizontal > 1) {
      while (blue[vertical][horizontal - blockcountL - 1]) { // left
        blockcountL++;
        if (horizontal - 1 - blockcountL < 0) {
          blockcountL = 0;
          break;
        }
        if (red[vertical][horizontal - blockcountL - 1]) {
          validflag++;
          break;
        }
        if (blue[vertical][horizontal - blockcountL - 1] == 0) {
          blockcountL = 0;
          break;
        }
      }
    }
    if (horizontal < dimension - 2) {
      while (blue[vertical][horizontal + blockcountR + 1]) { // right
        blockcountR++;
        if (horizontal + 1 + blockcountR > dimension - 1) {
          blockcountR = 0;
          break;
        }
        if (red[vertical][horizontal + blockcountR + 1]) {
          validflag++;
          break;
        }
        if (blue[vertical][horizontal + blockcountR + 1] == 0) {
          blockcountR = 0;
          break;
        }
      }
    }
  }

  return 0;
}
int ValidCorners() {
  if ((occupied == 0) && user == 0) { // blue turn
    if ((vertical > 1) && (horizontal > 1)) {
      while (red[vertical - 1 - blockcountNW]
                [horizontal - 1 - blockcountNW]) { // NW
        blockcountNW++;
        if ((vertical - 1 - blockcountNW < 0) ||
            (horizontal - 1 - blockcountNW < 0)) {
          blockcountNW = 0;
          break;
        }
        if (blue[vertical - 1 - blockcountNW][horizontal - 1 - blockcountNW]) {
          validflag++;
          break;
        }
        if (red[vertical - 1 - blockcountNW][horizontal - 1 - blockcountNW] ==
            0) {
          blockcountNW = 0;
          break;
        }
      }
    }
    if ((vertical > 1) && (horizontal < dimension - 2)) {
      while (red[vertical - 1 - blockcountNE]
                [horizontal + 1 + blockcountNE]) { // NE
        blockcountNE++;
        if ((vertical + 1 + blockcountNE < 0) ||
            (horizontal + 1 + blockcountNE > dimension - 1)) {
          blockcountNE = 0;
          break;
        }
        if (blue[vertical - 1 - blockcountNE][horizontal + 1 + blockcountNE]) {
          validflag++;
          break;
        }
        if (red[vertical - 1 - blockcountNE][horizontal + 1 + blockcountNE] ==
            0) {
          blockcountNE = 0;
          break;
        }
      }
    }
    if ((vertical < dimension - 2) && (horizontal > 1)) {
      while (red[vertical + 1 + blockcountSW]
                [horizontal - blockcountSW - 1]) { // SW
        blockcountSW++;
        if ((vertical + 1 + blockcountSW > dimension - 1) ||
            (horizontal - 1 - blockcountSW < 0)) {
          blockcountSW = 0;
          break;
        }
        if (blue[vertical + 1 + blockcountSW][horizontal - 1 - blockcountSW]) {
          validflag++;
          break;
        }
        if (red[vertical + 1 + blockcountSW][horizontal - 1 - blockcountSW] ==
            0) {
          blockcountSW = 0;
          break;
        }
      }
    }
    if ((vertical < dimension - 2) && (horizontal < dimension - 2)) {
      while (red[vertical + 1 + blockcountSE]
                [horizontal + blockcountSE + 1]) { // SE
        blockcountSE++;
        if ((vertical + 1 + blockcountSE > dimension - 1) ||
            (horizontal + 1 + blockcountSE > dimension - 1)) {
          blockcountSE = 0;
          break;
        }
        if (blue[vertical + 1 + blockcountSE][horizontal + blockcountSE + 1]) {
          validflag++;
          break;
        }
        if (red[vertical + 1 + blockcountSE][horizontal + blockcountSE + 1] ==
            0) {
          blockcountSE = 0;
          break;
        }
      }
    }
  }
  if ((occupied == 0) && user == 1) { // red turn
    if ((vertical > 1) && (horizontal > 1)) {
      while (blue[vertical - 1 - blockcountNW]
                 [horizontal - 1 - blockcountNW]) { // NW
        blockcountNW++;
        if ((vertical + 1 + blockcountNW < 0) ||
            (horizontal - 1 - blockcountNW < 0)) {
          blockcountNW = 0;
          break;
        }
        if (red[vertical - 1 - blockcountNW][horizontal - 1 - blockcountNW]) {
          validflag++;
          break;
        }
        if (blue[vertical - 1 - blockcountNW][horizontal - 1 - blockcountNW] ==
            0) {
          blockcountNW = 0;
          break;
        }
      }
    }
    if ((vertical > 1) && (horizontal < dimension - 2)) {
      while (blue[vertical - 1 - blockcountNE]
                 [horizontal + 1 + blockcountNE]) { // NE
        blockcountNE++;
        if ((vertical + 1 + blockcountNE < 0) ||
            (horizontal + 1 + blockcountNE > dimension - 1)) {
          blockcountNE = 0;
          break;
        }
        if (red[vertical - 1 - blockcountNE][horizontal + 1 + blockcountNE]) {
          validflag++;
          break;
        }
        if (blue[vertical - 1 - blockcountNE][horizontal + 1 + blockcountNE] ==
            0) {
          blockcountNE = 0;
          break;
        }
      }
    }
    if ((vertical < dimension - 2) && (horizontal > 1)) {
      while (blue[vertical + 1 + blockcountSW]
                 [horizontal - blockcountSW - 1]) { // SW
        blockcountSW++;
        if ((vertical + 1 + blockcountSW > dimension - 1) ||
            (horizontal - 1 - blockcountSW < 0)) {
          blockcountSW = 0;
          break;
        }
        if (red[vertical + 1 + blockcountSW][horizontal - 1 - blockcountSW]) {
          validflag++;
          break;
        }
        if (blue[vertical + 1 + blockcountSW][horizontal - 1 - blockcountSW] ==
            0) {
          blockcountSW = 0;
          break;
        }
      }
    }
    if ((vertical < dimension - 2) && (horizontal < dimension - 2)) {
      while (blue[vertical + 1 + blockcountSE]
                 [horizontal + blockcountSE + 1]) { // SE
        blockcountSE++;
        if ((vertical + 1 + blockcountSE > dimension - 1) ||
            (horizontal + 1 + blockcountSE > dimension - 1)) {
          blockcountSE = 0;
          break;
        }
        if (red[vertical + 1 + blockcountSE][horizontal + blockcountSE + 1]) {
          validflag++;
          break;
        }
        if (blue[vertical + 1 + blockcountSE][horizontal + blockcountSE + 1] ==
            0) {
          blockcountSE = 0;
          break;
        }
      }
    }
  }
  return 0;
}
int MoveSets(int Vmoves, int Hmoves) {
  blockcountU2 = 0;
  blockcountD2 = 0;
  blockcountL2 = 0;
  blockcountR2 = 0;
  blockcountNW2 = 0;
  blockcountNE2 = 0;
  blockcountSW2 = 0;
  blockcountSE2 = 0;
  MoveCount = 0;
  occupied2 = 0;
  if ((red[Vmoves][Hmoves] == 1) || (blue[Vmoves][Hmoves] == 1)) {
    occupied2 = 1;
  }
  if ((occupied2 == 0) && user == 0) { // blue turn
    if (Vmoves > 1) {
      while (red[Vmoves - 1 - blockcountU2][Hmoves]) { // up
        blockcountU2++;
        if (Vmoves - 1 - blockcountU2 < 0) {
          blockcountU = 0;
          break;
        }
        if (blue[Vmoves - 1 - blockcountU2][Hmoves]) {
          MoveCount++;
          break;
        }

        if (red[Vmoves - 1 - blockcountU2][Hmoves] == 0) {
          blockcountU2 = 0;
          break;
        }
      }
    }
    if (Vmoves < dimension - 2) {
      while (red[Vmoves + 1 + blockcountD2][Hmoves]) { // down
        blockcountD2++;
        if (Vmoves + 1 + blockcountD2 > dimension - 1) {
          blockcountD2 = 0;
          break;
        }
        if (blue[Vmoves + 1 + blockcountD2][Hmoves]) {
          MoveCount++;
          break;
        }
        if (red[Vmoves + 1 + blockcountD2][Hmoves] == 0) {
          blockcountD2 = 0;
          break;
        }
      }
    }
    if (Hmoves > 1) {
      while (red[Vmoves][Hmoves - blockcountL2 - 1]) { // left
        blockcountL2++;
        if (Hmoves - 1 - blockcountL2 < 0) {
          blockcountL2 = 0;
          break;
        }
        if (blue[Vmoves][Hmoves - blockcountL2 - 1]) {
          MoveCount++;
          break;
        }
        if (red[Vmoves][Hmoves - blockcountL2 - 1] == 0) {
          blockcountL2 = 0;
          break;
        }
      }
    }
    if (Hmoves < dimension - 2) {
      while (red[Vmoves][Hmoves + blockcountR2 + 1]) { // right
        blockcountR2++;
        if (Hmoves + 1 + blockcountR2 > dimension - 1) {
          blockcountR2 = 0;
          break;
        }
        if (blue[Vmoves][Hmoves + blockcountR2 + 1]) {
          MoveCount++;
          break;
        }
        if (red[Vmoves][Hmoves + blockcountR2 + 1] == 0) {
          blockcountR2 = 0;
          break;
        }
      }
    }
  }
  if ((occupied2 == 0) && user == 1) { // red turn
    if (Vmoves > 1) {
      while (blue[Vmoves - 1 - blockcountU2][Hmoves]) { // up
        blockcountU2++;
        if (Vmoves - 1 - blockcountU2 < 0) {
          blockcountU2 = 0;
          break;
        }
        if (red[Vmoves - 1 - blockcountU2][Hmoves]) {
          MoveCount++;
          break;
        }
        if (blue[Vmoves - 1 - blockcountU2][Hmoves] == 0) {
          blockcountU2 = 0;
          break;
        }
      }
    }
    if (Vmoves < dimension - 2) {
      while (blue[Vmoves + 1 + blockcountD2][Hmoves]) { // down
        blockcountD2++;
        if (Vmoves + 1 + blockcountD2 > dimension - 1) {
          blockcountD2 = 0;
          break;
        }
        if (red[Vmoves + 1 + blockcountD2][Hmoves]) {
          MoveCount++;
          break;
        }
        if (blue[Vmoves + 1 + blockcountD2][Hmoves] == 0) {
          blockcountD2 = 0;
          break;
        }
      }
    }
    if (Hmoves > 1) {
      while (blue[Vmoves][Hmoves - blockcountL2 - 1]) { // left
        blockcountL2++;
        if (Hmoves - 1 - blockcountL2 < 0) {
          blockcountL2 = 0;
          break;
        }
        if (red[Vmoves][Hmoves - blockcountL2 - 1]) {
          MoveCount++;
          break;
        }
        if (blue[Vmoves][Hmoves - blockcountL2 - 1] == 0) {
          blockcountL2 = 0;
          break;
        }
      }
    }
    if (Hmoves < dimension - 2) {
      while (blue[Vmoves][Hmoves + blockcountR2 + 1]) { // right
        blockcountR2++;
        if (Hmoves + 1 + blockcountR2 > dimension - 1) {
          blockcountR2 = 0;
          break;
        }
        if (red[Vmoves][Hmoves + blockcountR2 + 1]) {
          MoveCount++;
          break;
        }
        if (blue[Vmoves][Hmoves + blockcountR2 + 1] == 0) {
          blockcountR2 = 0;
          break;
        }
      }
    }
  }
  ///////////////////////////////////////////////////////////////////////////////////NW
  /// NE SW SW
  if ((occupied2 == 0) && user == 0) { // blue turn
    if ((Vmoves > 1) && (Hmoves > 1)) {
      while (
          red[Vmoves - 1 - blockcountNW2][Hmoves - 1 - blockcountNW2]) { // NW
        blockcountNW2++;
        if ((Vmoves - 1 - blockcountNW2 < 0) ||
            (Hmoves - 1 - blockcountNW2 < 0)) {
          blockcountNW2 = 0;
          break;
        }
        if (blue[Vmoves - 1 - blockcountNW2][Hmoves - 1 - blockcountNW2]) {
          MoveCount++;
          break;
        }
        if (red[Vmoves - 1 - blockcountNW2][Hmoves - 1 - blockcountNW2] == 0) {
          blockcountNW2 = 0;
          break;
        }
      }
    }
    if ((Vmoves > 1) && (Hmoves < dimension - 2)) {
      while (
          red[Vmoves - 1 - blockcountNE2][Hmoves + 1 + blockcountNE2]) { // NE
        blockcountNE2++;
        if ((Vmoves + 1 + blockcountNE2 < 0) ||
            (Hmoves + 1 + blockcountNE2 > dimension - 1)) {
          blockcountNE2 = 0;
          break;
        }
        if (blue[Vmoves - 1 - blockcountNE2][Hmoves + 1 + blockcountNE2]) {
          MoveCount++;
          break;
        }
        if (red[Vmoves - 1 - blockcountNE2][Hmoves + 1 + blockcountNE2] == 0) {
          blockcountNE2 = 0;
          break;
        }
      }
    }
    if ((Vmoves < dimension - 2) && (Hmoves > 1)) {
      while (
          red[Vmoves + 1 + blockcountSW2][Hmoves - blockcountSW2 - 1]) { // SW
        blockcountSW2++;
        if ((Vmoves + 1 + blockcountSW2 > dimension - 1) ||
            (Hmoves - 1 - blockcountSW2 < 0)) {
          blockcountSW2 = 0;
          break;
        }
        if (blue[Vmoves + 1 + blockcountSW2][Hmoves - 1 - blockcountSW2]) {
          MoveCount++;
          break;
        }
        if (red[Vmoves + 1 + blockcountSW2][Hmoves - 1 - blockcountSW2] == 0) {
          blockcountSW2 = 0;
          break;
        }
      }
    }
    if ((Vmoves < dimension - 2) && (Hmoves < dimension - 2)) {
      while (
          red[Vmoves + 1 + blockcountSE2][Hmoves + blockcountSE2 + 1]) { // SE
        blockcountSE2++;
        if ((Vmoves + 1 + blockcountSE2 > dimension - 1) ||
            (Hmoves + 1 + blockcountSE2 > dimension - 1)) {
          blockcountSE2 = 0;
          break;
        }
        if (blue[Vmoves + 1 + blockcountSE2][Hmoves + blockcountSE2 + 1]) {
          MoveCount++;
          break;
        }
        if (red[Vmoves + 1 + blockcountSE2][Hmoves + blockcountSE2 + 1] == 0) {
          blockcountSE2 = 0;
          break;
        }
      }
    }
  }
  if ((occupied2 == 0) && user == 1) { // red turn
    if ((Vmoves > 1) && (Hmoves > 1)) {
      while (
          blue[Vmoves - 1 - blockcountNW2][Hmoves - 1 - blockcountNW2]) { // NW
        blockcountNW2++;
        if ((Vmoves + 1 + blockcountNW2 < 0) ||
            (Hmoves - 1 - blockcountNW2 < 0)) {
          blockcountNW2 = 0;
          break;
        }
        if (red[Vmoves - 1 - blockcountNW2][Hmoves - 1 - blockcountNW2]) {
          MoveCount++;
          break;
        }
        if (blue[Vmoves - 1 - blockcountNW2][Hmoves - 1 - blockcountNW2] == 0) {
          blockcountNW2 = 0;
          break;
        }
      }
    }
    if ((Vmoves > 1) && (Hmoves < dimension - 2)) {
      while (
          blue[Vmoves - 1 - blockcountNE2][Hmoves + 1 + blockcountNE2]) { // NE
        blockcountNE2++;
        if ((Vmoves + 1 + blockcountNE2 < 0) ||
            (Hmoves + 1 + blockcountNE2 > dimension - 1)) {
          blockcountNE2 = 0;
          break;
        }
        if (red[Vmoves - 1 - blockcountNE2][Hmoves + 1 + blockcountNE2]) {
          MoveCount++;
          break;
        }
        if (blue[Vmoves - 1 - blockcountNE2][Hmoves + 1 + blockcountNE2] == 0) {
          blockcountNE2 = 0;
          break;
        }
      }
    }
    if ((Vmoves < dimension - 2) && (Hmoves > 1)) {
      while (
          blue[Vmoves + 1 + blockcountSW2][Hmoves - blockcountSW2 - 1]) { // SW
        blockcountSW2++;
        if ((Vmoves + 1 + blockcountSW2 > dimension - 1) ||
            (Hmoves - 1 - blockcountSW2 < 0)) {
          blockcountSW2 = 0;
          break;
        }
        if (red[Vmoves + 1 + blockcountSW2][Hmoves - 1 - blockcountSW2]) {
          MoveCount++;
          break;
        }
        if (blue[Vmoves + 1 + blockcountSW2][Hmoves - 1 - blockcountSW2] == 0) {
          blockcountSW2 = 0;
          break;
        }
      }
    }
    if ((Vmoves < dimension - 2) && (Hmoves < dimension - 2)) {
      while (
          blue[Vmoves + 1 + blockcountSE2][Hmoves + blockcountSE2 + 1]) { // SE
        blockcountSE2++;
        if ((Vmoves + 1 + blockcountSE2 > dimension - 1) ||
            (Hmoves + 1 + blockcountSE2 > dimension - 1)) {
          blockcountSE2 = 0;
          break;
        }
        if (red[Vmoves + 1 + blockcountSE2][Hmoves + blockcountSE2 + 1]) {
          MoveCount++;
          break;
        }
        if (blue[Vmoves + 1 + blockcountSE2][Hmoves + blockcountSE2 + 1] == 0) {
          blockcountSE2 = 0;
          break;
        }
      }
    }
  }
  return MoveCount;
}

int Debug() {
  // LED_Write(flag2%2);
  LCD_Position(0u, 0u);
  LCD_PrintString("U");
  LCD_Position(0u, 1u);
  LCD_PrintNumber(blockcountU);

  LCD_Position(0u, 2u);
  LCD_PrintString("D");
  LCD_Position(0u, 3u);
  LCD_PrintNumber(blockcountD);

  LCD_Position(0u, 4u);
  LCD_PrintString("L");
  LCD_Position(0u, 5u);
  LCD_PrintNumber(blockcountL);

  LCD_Position(0u, 6u);
  LCD_PrintString("R");
  LCD_Position(0u, 7u);
  LCD_PrintNumber(blockcountR);

  LCD_Position(1u, 0u);
  LCD_PrintString("NW");
  LCD_Position(1u, 2u);
  LCD_PrintNumber(blockcountNW);

  LCD_Position(1u, 3u);
  LCD_PrintString("NE");
  LCD_Position(1u, 5u);
  LCD_PrintNumber(blockcountNE);

  LCD_Position(1u, 6u);
  LCD_PrintString("SW");
  LCD_Position(1u, 8u);
  LCD_PrintNumber(blockcountSW);

  LCD_Position(1u, 9u);
  LCD_PrintString("SE");
  LCD_Position(1u, 11u);
  LCD_PrintNumber(blockcountSE);

  LCD_Position(1u, 12u);
  LCD_PrintNumber(valid);
  return 0;
}
int Avail() {
  if (Show == 1 || RefreshShow) {
    for (i = 0; i < dimension; i++) {
      for (j = 0; j < dimension; j++) {
        green[i][j] = 0;
      }
    }
    for (i = 0; i < dimension; i++) {
      for (j = 0; j < dimension; j++) {
        MoveFlag = MoveSets(i, j);
        if (MoveFlag) {
          green[i][j] = 1;
        }
      }
    }
    Show = 0;
    RefreshShow = 0;
  }
  if (DontShow == 1) {
    for (i = 0; i < dimension; i++) {
      for (j = 0; j < dimension; j++) {
        green[i][j] = 0;
      }
    }
    DontShow = 0;
  }
  return 0;
}
int EndConditions() {
  if (PossibleFlag) {
    MoveTot = 0;
    for (i = 0; i < dimension; i++) {
      for (j = 0; j < dimension; j++) {
        MoveFlag = MoveSets(i, j);
        if (MoveFlag) {
          MoveTot++;
        }
      }
    }
    PossibleFlag = 0;
    if (MoveTot == 0) {
      Possible = 0;
    }
  }
  if (Possible == 0) {
    usercount++;
    user = usercount % 2;
    MoveTot2 = 0;
    for (i = 0; i < dimension; i++) {
      for (j = 0; j < dimension; j++) {
        MoveFlag = MoveSets(i, j);
        if (MoveFlag) {
          MoveTot2++;
        }
      }
    }
    if (MoveTot2 == 0) {
      LED_Write(1);
    }
    LCD_Position(1u, 0u);
    LCD_PrintNumber(MoveTot);
    Possible = 1;
  }
  return 0;
}
int WriteLED() {
  if (flag == 1) {

    if ((large == 1) && (row == (dimension / 2))) {
      row = 0;
    } else if (row == (dimension)) {
      row = 0;
    }
    if (usercount % 2 == 1) {
      colorcursor = 0b100;
    } else {
      colorcursor = 0b001;
    }
    OE_Write(1);
    PL_Write(0);
    clk_Write(0);
    Rows_Write(row);
    for (column = 0; column < 32; column++) {
      mask = 0;
      mask2 = 0;
      occupied = 0;
      if (column < (dimension)) {
        if (red[row][column] == 1) {
          mask = mask | 0b100;
        }
        if (green[row][column] == 1) {
          mask = mask | 0b010;
        }
        if (blue[row][column] == 1) {
          mask = mask | 0b001;
        }
        if (large == 1) {
          if (red[row + 8][column] == 1) {
            mask2 = mask2 | 0b100;
          }
          if (green[row + 8][column] == 1) {
            mask2 = mask2 | 0b010;
          }
          if (blue[row + 8][column] == 1) {
            mask2 = mask2 | 0b001;
          }
        }
      }
      if (large == 1) {
        if ((row == (down - up)) && (column == (horizontal))) {
          if ((red[vertical][horizontal] == 1) ||
              (blue[vertical][horizontal] == 1)) {
            occupied = 1;
          }
          if (flag2 % 2 == 0) {
            mask = 0;
          }
          if ((flag2 % 2 == 1) && (occupied == 0)) {
            mask = colorcursor;
          }
        }
        if (((vertical) >= (dimension / 2)) && (row == (down - up) % 8) &&
            (column == (horizontal))) {
          if ((red[vertical][horizontal] == 1) ||
              (blue[vertical][horizontal] == 1)) {
            occupied = 1;
          }
          if (flag2 % 2 == 0) {
            mask2 = 0;
          }
          if ((flag2 % 2 == 1) && (occupied == 0)) {
            mask2 = colorcursor;
          }
        }
      } else {
        if ((row == (down - up)) && (column == (horizontal))) {
          if ((red[vertical][horizontal] == 1) ||
              (blue[vertical][horizontal] == 1)) {
            occupied = 1;
          }
          if (flag2 % 2 == 0) {
            mask = 0;
          }
          if ((flag2 % 2 == 1) && (occupied == 0)) {
            mask = colorcursor;
          }
        }
      }
      RGB_Write(mask);
      RGB2_Write(mask2);
      clk_Write(1);
      clk_Write(0);
    }
    PL_Write(1);
    PL_Write(0);
    OE_Write(0);

    flag = 0;
    row++;
    // Debug();
  }
  return 0;
}
/* [] END OF FILE */
