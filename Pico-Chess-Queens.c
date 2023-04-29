/* ================================================================== *\
   Pico-Chess-Queens.c
   St-Louys Andre - April 2023
   astlouys@gmail.com
   Revision 13-APR-2023
   Compiler: GNU 7.3.1
   Version 1.00

   "The chess queens problem..."
   You're asked to "safely" place eight (8) queens on a chess game. "Safely" because there must be
   only one queen per line, one queen per column and one queen per diagonal...
   How many solutions are possible ?


   REVISION HISTORY:
   =================
   23-APR- 2023 1.00 - Initial release.
\* ================================================================== */



/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                                Include files.
\* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "math.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"




/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                                Definitions.
\* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
typedef unsigned int  UINT;   // processor-optimized.
typedef uint8_t       UINT8;
typedef uint16_t      UINT16;
typedef uint32_t      UINT32;
typedef uint64_t      UINT64;
typedef unsigned char UCHAR;


#define FAULT_SAME_COLUMN     1
#define FAULT_DIAGONAL_AHEAD  2
#define FAULT_DIAGONAL_BEHIND 3


#define FAILED            0
#define MAX_COLUMNS       8
#define MAX_ROWS          8
#define PASSED            1
#define PICO_LED         25
#define MODE_ALL_MATRIX   1
#define MODE_VALID_MATRIX 2



/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                              Global variables.
\* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- */



/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                             Function prototypes.
\* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* Blink Pico's LED the specified number of times. */
void blink_pico_led(UINT8 NumberOfTimes);

/* Display matrix whose pointer is passed as an argument. */
void display_matrix(UINT8 Row[MAX_ROWS]);

/* Read a string from stdin. */
void input_string(UCHAR *String);

/* Check if matrix passed as an argument is valid. */
UINT8 validate_matrix(UINT8 Row[MAX_ROWS], UINT8 SoftwareMode, UINT8 *FaultId, UINT8 *Row1, UINT8 *Row2);



/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                          Main program entry point.
\* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
int main(void)
{
  UCHAR DataInput;
  UCHAR String[256];

  UINT8 Column;
  UINT8 FaultId;
  UINT8 FlagPassed;
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 Menu;
  UINT8 Row1;
  UINT8 Row2;
  UINT8 Row[8];
  UINT8 SoftwareMode;

  UINT32 MatrixNumber;
  UINT32 TotalSolutions;

  uart_inst_t *Uart;  // Pico's UART used to serially transfer data to an external monitor or to a PC.


  /* Initialize UART0 used for bi-directional communication with a PC running TeraTermn (or other) terminal emulation software. */
  stdio_init_all();
  uart_init(uart0, 921600);


  // gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  // gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
  uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

  
  /* Initialize gpio for Pico's LED pin. */
  gpio_init(PICO_LED);
  gpio_set_dir(PICO_LED, GPIO_OUT);


  /* ---------------------------------------------------------------- *\
               Wait for CDC USB to establish connection.
  \* ---------------------------------------------------------------- */
  while (!stdio_usb_connected())
  {
    blink_pico_led(3);

    sleep_ms(1200);
  }


  printf("===========================\r");
  printf("The Chess Queens problem...\r");
  printf("===========================\r\r");
  printf("You're asked to <safely> place eight (8) queens on a chess game.\r");
  printf("<Safely> because there must be only one queen per row, one queen per column and one queen per diagonal...\r");
  printf("How many solutions are possible ?\r\r");
  printf("Press <1> to display only the valid solutions.\r");
  printf("Press <2> to display all matrix evaluated and why many of them are rejected.\r\r");
  printf("Option 1 takes about 20 seconds to complete and it shows its progression during execution.\r");
  printf("Option 2 takes more then two days of processing and about 9 GBytes of disk space if you log the data.\r");
  printf("However, option 2 allows you to analyze and understand the logic behind the brute force algorithm used.\r");
  printf("Also, if you select option 2, every now and then, you will be asked to open a new log file\r");
  printf("(in order to have many small log files instead of a single huge 8 GB file). When asked to open a new log file,\r");
  printf("abort your terminal emulator program, rename the log file (preferably with a filename including a sequential number)\r");
  printf("then restart the terminal emulator program and press <Enter> (the program should resume where it left).\r");
  printf("Of course, many different algorithms may be used. The one used in this program is more of a <brute force method>\r");
  printf("than an optimized one. Let me know if you explore other algorithms.\r\r\r");
  printf("Enter your choice <1> or <2>: ");

  do
  {
    input_string(String);
  } while (String[0] == 0x0D);
  Menu = atoi(String);
    
    
  switch(Menu)
  {
    case (1):
      /* Display only valid solution matrix. */
      printf("\r\r");
      SoftwareMode = MODE_VALID_MATRIX;
      printf("\r\r");
    break;
      
     case (2):
      /* Display all matrix being evaluated. */
      printf("\r\r");
      SoftwareMode = MODE_ALL_MATRIX;
      printf("\r\r");
     break;
      
    default:
      printf("\r\r");
    break;
  }



  /* ---------------------------------------------------------------- *\
                            Initializations.
  \* ---------------------------------------------------------------- */
  /* Initialize total number of valid solutions. */
  TotalSolutions = 0;

  /* Initialize total number of matrix tested. */
  MatrixNumber = 0;

  /* Initialize matrix. */
  for (Loop1UInt8 = 0; Loop1UInt8 < MAX_ROWS; ++Loop1UInt8)
    Row[Loop1UInt8] = 0;

  printf("================================================================\r");
  printf("Start matrix:\r\r");
  display_matrix(Row);
  printf("================================================================\r\r");
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r\r\r");



  /* ---------------------------------------------------------------- *\
                           Brute force method...
       Scan all possible combinations (assuming one queen per row),
            and test if this combination is a valid solution.
  \* ---------------------------------------------------------------- */
  for (Row[0] = 0; Row[0] < MAX_ROWS; ++Row[0])
  {
    if ((SoftwareMode == MODE_ALL_MATRIX) && (Row[0] != 0))
    {
      printf("You may want to open a new log file at this point...\r");
      printf("Press a key to continue: ");

      /* ---------------------------------------------------------------- *\
                Blink Pico's LED when this loop is changing value
            to indicate to user that it's time to open next log file.
                    Wait for user to press <Enter> to continue.
      \* ---------------------------------------------------------------- */
      Loop1UInt8 = 0;
      do
      {
        DataInput = getchar_timeout_us(50000);
        sleep_ms(100);
        ++Loop1UInt8;
        if (Loop1UInt8 > 100)
        {
          Loop1UInt8 = 0;
          blink_pico_led(3);
        }
      } while(DataInput != 0x0D);
    }

    for (Row[1] = 0; Row[1] < MAX_ROWS; ++Row[1])
    {
      for (Row[2] = 0; Row[2] < MAX_ROWS; ++Row[2])
      {
        for (Row[3] = 0; Row[3] < MAX_ROWS; ++Row[3])
        {
          for (Row[4] = 0; Row[4] < MAX_ROWS; ++Row[4])
          {
            for (Row[5] = 0; Row[5] < MAX_ROWS; ++Row[5])
            {
              for (Row[6] = 0; Row[6] < MAX_ROWS; ++Row[6])
              {
                for (Row[7] = 0; Row[7] < MAX_ROWS; ++Row[7])
                {
                  ++MatrixNumber;
                  FlagPassed = validate_matrix(Row, SoftwareMode, &FaultId, &Row1, &Row2);
                  if (FlagPassed == PASSED)
                  {
                    ++TotalSolutions;

                    printf("\r");
                    printf("================================================================\r");
                     printf("                       Solution number %u\r", TotalSolutions);
                    printf("                  Current matrix number %u\r\r", MatrixNumber);
                    display_matrix(Row);
                    printf("                     *** VALID SOLUTION ***\r");
                    printf("================================================================\r\r\r");
                  }
                  else
                  {
                    if (SoftwareMode == MODE_VALID_MATRIX)
                    {
                      if ((MatrixNumber % 100000) == 0)
                        printf("Evaluating matrix number %8u\r", MatrixNumber);
                    }
                    else
                    {
                      printf("\r\r");
                      printf("================================================================\r");
                      printf("           Number of valid solutions found so far: %u\r", TotalSolutions);
                      printf("                  Current matrix number %u\r\r", MatrixNumber);
                      display_matrix(Row);
                      switch (FaultId)
                      {
                        case (FAULT_SAME_COLUMN):
                          printf("     Rejected because Row %u is in the same column as Row %u\r", Row1, Row2);
                        break;

                        case (FAULT_DIAGONAL_AHEAD):
                          printf(" Rejected because Row %u is on the same diagonal ahead as Row %u\r", Row1, Row2);
                        break;

                        case (FAULT_DIAGONAL_BEHIND):
                          printf(" Rejected because Row %u is on the same diagonal behind as Row %u\r", Row1, Row2);
                        break;
                      }
                      printf("                             FAILED\r");
                      printf("================================================================\r\r\r");

                      /// printf("Press <Enter> to continue: ");
                      /// input_string(String);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  printf("\r");
  printf("Total number of matrix checked: %u  (8^8 = %f)\r\r", MatrixNumber, pow(8, 8));
  printf("Total number of valid solutions found: %u\r\r", TotalSolutions);
  printf("Program ended...\r\r");


  return 0;
}





/* $PAGE */
/* $TITLE=blink_pico_led() */
/* ------------------------------------------------------------------ *\
            Blink Pico's LED the specified number of times.
\* ------------------------------------------------------------------ */
void blink_pico_led(UINT8 NumberOfTimes)
{
  UINT8 Loop1UInt8;


  for (Loop1UInt8 = 0; Loop1UInt8 < NumberOfTimes; ++Loop1UInt8)
  {
    gpio_put(PICO_LED, true);
    sleep_ms(120);
    gpio_put(PICO_LED, false);
    sleep_ms(300);
  }

  return;
}





/* $PAGE */
/* $TITLE=display_matrix() */
/* ------------------------------------------------------------------ *\
         Display matrix whose pointer is passed as an argument.
\* ------------------------------------------------------------------ */
void display_matrix(UINT8 MatrixRow[MAX_ROWS])
{
  UINT8 Row;
  UINT8 Column;


  for (Row = 0; Row < MAX_ROWS; ++Row)
  {
    printf("            ");
    for (Column = 0; Column < MAX_COLUMNS; ++Column)
    {
      if (MatrixRow[Row] == Column)
        printf("  @  ");
      else
        printf("  -  ");
    }
    printf("\r\r");
  }

  return;
}





/* $PAGE */
/* $TITLE=input_string() */
/* ------------------------------------------------------------------ *\
                       Read a string from stdin.
\* ------------------------------------------------------------------ */
void input_string(UCHAR *String)
{
  int8_t DataInput;

  UINT8 Loop1UInt8;


  Loop1UInt8 = 0;
  do
  {
    DataInput = getchar_timeout_us(50000);

    switch (DataInput)
    {
      case (PICO_ERROR_TIMEOUT):
      case (0):
        continue;
      break;

      case (8):
        /* <Backspace> */
        if (Loop1UInt8 > 0)
        {
          --Loop1UInt8;
          String[Loop1UInt8] = 0x00;
          printf("%c %c", 0x08, 0x08);  // erase character under the cursor.
        }
      break;

      case (0x0D):
        /* <Enter> */
        if (Loop1UInt8 == 0)
        {
          String[Loop1UInt8++] = (UCHAR)DataInput;  
          String[Loop1UInt8++] = 0x00;
        }
        printf("\r");
      break;

      default:
        printf("%c", (UCHAR)DataInput);
        String[Loop1UInt8] = (UCHAR)DataInput;
        // printf("Loop1UInt8: %3u   %2.2X - %c\r", Loop1UInt8, DataInput, DataInput);  ///
        ++Loop1UInt8;
      break;
    }
  } while((Loop1UInt8 < 128) && (DataInput != 0x0D));
  
  String[Loop1UInt8] = '\0';  // end-of-string

  /***
  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
    printf("%2u:[%2.2X]   ", Loop1UInt8, String[Loop1UInt8]);
  printf("\r");
  ***/

  return;
}





/* $PAGE */
/* $TITLE=validate_matrix() */
/* ------------------------------------------------------------------ *\
       Check if matrix passed as an argument is a valid solution.
\* ------------------------------------------------------------------ */
UINT8 validate_matrix(UINT8 MatrixRow[MAX_ROWS], UINT8 SoftwareMode, UINT8 *FaultId, UINT8 *FirstRow, UINT8 *SecondRow)
{
  UINT8 Row1;
  UINT8 Row2;


  for (Row1 = 0; Row1 < MAX_ROWS; ++Row1)
  {
    for (Row2 = Row1 + 1; Row2 < MAX_ROWS; ++Row2)
    {
      if (MatrixRow[Row2] == MatrixRow[Row1])
      {
        *FaultId = FAULT_SAME_COLUMN;
        *FirstRow  = Row1;
        *SecondRow = Row2;
        /// printf("     Rejected because Row %u is in the same column as Row %u\r", Row1, Row2);

        return FAILED;  // same column.
      }

      if (MatrixRow[Row2] == MatrixRow[Row1] + (Row2 - Row1)) 
      {
        *FaultId = FAULT_DIAGONAL_AHEAD;
        *FirstRow  = Row1;
        *SecondRow = Row2;
        /// printf(" Rejected because Row %u is on the same diagonal ahead as Row %u\r", Row1, Row2);

        return FAILED;  // same diagonal ahead.
      }

      if (MatrixRow[Row2] == MatrixRow[Row1] - (Row2 - Row1)) 
      {
        *FaultId = FAULT_DIAGONAL_BEHIND;
        *FirstRow  = Row1;
        *SecondRow = Row2;
        /// printf(" Rejected because Row %u is on the same diagonal behind as Row %u\r", Row1, Row2);
        
        return FAILED;  // same diagonal ahead.
      }
    }
  }

  return PASSED;
}
