/*
    revers.h - Example for KNX module (KIM) library
    Copyright (C) 2021  Fabio Di MIchele
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

//Calculate array size
#define ARRAY_SIZE(a)  sizeof(a)/sizeof(a[0])

enum KIMeventID {
  SYS_EVENT           = 0x0100,
  CON_TRANSPOPT_LAYER = 0x0200,
  LOAD_STATE_MACHINE  = 0x0300,
  OBJ_OK              = 0x0402,
  OBJ_ERROR           = 0x0403,
  OBJ_NO              = 0x0410
};

//Function to reverse arr[] from start to end
void revereseArray(char arr[], size_t size)
{
    for (size_t i = 0; i < size/2; i++)
    {
        char temp = arr[i];
        arr[i] = arr[size - 1 - i];
        arr[size - 1 - i] = temp;
    }
}

//print the array
void printArray(char arr[], size_t size)
{
    for (size_t i=0; i < size; i++)
    {
      if (i > 0) Serial.print(F(" "));
      Serial.print(arr[i], HEX);
    }
    //Serial.println();
}

void cleanArray(char arr[], size_t size)
{
    for (size_t i=0; i < size; i++)
    {
        arr[i] = '\0';
    }
}
