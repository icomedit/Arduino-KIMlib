/*
 * This example check if the firmware loaded on the TAPKO KIMaip
 * module is updated.
 *
 * Circuit:
 * - KIMaip Module attached
 *
 * Created 16 November 2021 by Fabio Di Michele
 * This code is in the public domain.
 */

static String priorityMode(byte mode)
{
  switch (mode) {
    case 0:           
        return "ETS";
        break;
    case 1:           
        return "I2C";
        break;
    case 2:           
        return "Default";
        break;
  } 
}

static String sino(byte mode)
{
  if (mode == 0) 
    return "NO";
  else     
    return "YES";
}
