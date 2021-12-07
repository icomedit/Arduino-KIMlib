/*
    calendarClock.h - Example for KNX module (KIM) library

    This include a internal time clock and get date and time to compile.

    Copyright (C) 2021  Fabio Di Michele
    Copyright (C) 2021  Giulio Paggi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#define LEAP_YEAR(Y)    ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ))     // from time-lib
#define YEAR_OFFSET     1900

byte hour = 0;

struct Date_Time {
  byte lsb   = B00000000;
  byte flags = B00000000;
  byte seconds = 0;
  byte minutes = 0;
  byte dayHour = 0;
  byte day   = 1;
  byte month = 1;
  byte year  = 0;
} datetime;

void compileTime() {
  const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char chMon[4], *m;

  strncpy(chMon, compDate, 3);
  chMon[3] = '\0';
  m = strstr(months, chMon);
  datetime.month = ((m - months) / 3 + 1);
  datetime.day = atoi(compDate + 4);
  datetime.year = atoi(compDate + 7) - YEAR_OFFSET;
  hour = atoi(compTime);
  datetime.minutes = atoi(compTime + 3);
  datetime.seconds = atoi(compTime + 6);
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10)
    Serial.print(F("0"));
  Serial.print(digits);
}

int dayOfWeek()
{
  uint16_t year = datetime.year + YEAR_OFFSET;
  uint16_t months[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365         };   // days until 1st of month

  uint32_t days = year * 365;        // days until year
  for (uint16_t i = 4; i < year; i += 4) if (LEAP_YEAR(i) ) days++;     // adjust leap years, test only multiple of 4 of course

  days += months[datetime.month-1] + datetime.day;    // add the days of this year
  if ((datetime.month > 2) && LEAP_YEAR(year)) days++;  // adjust 1 if this year is a leap year, but only after febr

  return days % 7;   // remove all multiples of 7
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(datetime.year + YEAR_OFFSET);
  Serial.print(F("/"));
  printDigits(datetime.month);
  Serial.print(F("/"));
  printDigits(datetime.day);
  Serial.print(F(" "));
  Serial.print(dayOfWeek());
  Serial.print(F(" (0x"));
  Serial.print(datetime.dayHour, HEX);
  Serial.print(F(") "));
  printDigits(hour);
  Serial.print(F(":"));
  printDigits(datetime.minutes);
  Serial.print(F(":"));
  printDigits(datetime.seconds);
  Serial.println();
}

/* A Function to return the number of days in
   a month

  Month Number     Name        Number of Days
  1                January     31
  2                February    28 (non-leap) / 29 (leap)
  3                March       31
  4                April       30
  5                May         31
  6                June        30
  7                July        31
  8                August      31
  9                September   30
  10               October     31
  11               November    30
  12               December    31

*/
int numberOfDays ()
{
  uint16_t year = datetime.year + YEAR_OFFSET;

  // January
  if (datetime.month == 1)
      return (31);

  // February
  if (datetime.month == 2)
  {
      // If the year is leap then February has
      // 29 days
      //if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
      if (LEAP_YEAR(year))
          return (29);
      else
          return (28);
  }

  // March
  if (datetime.month == 3)
      return (31);

  // April
  if (datetime.month == 4)
      return (30);

  // May
  if (datetime.month == 5)
      return (31);

  // June
  if (datetime.month == 6)
      return (30);

  // July
  if (datetime.month == 7)
      return (31);

  // August
  if (datetime.month == 8)
      return (31);

  // September
  if (datetime.month == 9)
      return (30);

  // October
  if (datetime.month == 10)
      return (31);

  // November
  if (datetime.month == 11)
      return (30);

  // December
  if (datetime.month == 12)
      return (31);
}

void tickClock() {

  if (datetime.seconds < 59) {
    datetime.seconds++;
  } else {
    datetime.seconds = 0;
    if (datetime.minutes < 59) {
      datetime.minutes++;
    } else {
      datetime.minutes = 0;
      if (hour < 23) {
        hour++;
      } else {
        hour = 0;
        if (datetime.day < numberOfDays()) {
          datetime.day++;
        } else {
          datetime.day = 1;
          if (datetime.month < 12) {
            datetime.month++;
          } else {
            datetime.month = 1;
            datetime.year++;
          }
        }
      }
    }
  }
  byte dayNumber = dayOfWeek() << 5;
  datetime.dayHour = dayNumber + hour;
}
