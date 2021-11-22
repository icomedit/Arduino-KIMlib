/*
    conversion.ino - Example for KNX module (KIM) library
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

union u_tag {
    uint16_t temp;
    byte temp_byte[2] ;
  }u;
  
uint16_t float2half (float f) {
  float v = f * 100.0f;
  int exponent = 0;
  for (; v < -2048.0f; v /= 2) exponent++;
  for (; v > 2047.0f; v /= 2) exponent++;  
  long m = round(v) & 0x7FF;
  short msb = (short) (exponent << 3 | m >> 8);
  if (f < 0.0f) msb |= 0x80;
  u.temp_byte[1] = msb;
  u.temp_byte[0] = (byte)m;
  return u.temp;  
}

float half2float (uint16_t halfFloat) {
  u.temp = halfFloat;
  int exponent = (u.temp_byte[0] & B01111000) >> 3;
  int mantissa = ((u.temp_byte[0] & B00000111) << 8) | u.temp_byte[1];

  if(u.temp_byte[0] & B10000000) {
    return ((-2048 + mantissa) * 0.01) * pow(2.0, exponent);
  }
  return (mantissa * 0.01) * pow(2.0, exponent);
}
