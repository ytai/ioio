#ifndef __INCAP_H__
#define __INCAP_H__


void InCapInit();

// mode:
//   0: high-to-low
//   1: low-to-high
//   2: high-to-high
//   3: high-to-high x 4
//   4: high-to-high x 16
//   5: low-to-low
//
// clock_scale:
//   0: off
//   1: 16MHz
//   2: 256KHz
//   3: 62.5KHz
void InCapConfig(int incap_num, int mode, int continouos, int clock_scale,
                 int input_scale);


#endif  // __INCAP_H__
