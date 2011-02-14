// Implements A/D-related functions of the protocol.
// Usage:
// ADCInit();
// ADCSetScan(pin_num);
// ...from now on will send periodic samples of pin_num...
// ADCClrScan(pin_num);
// ...will no longer send samples...


#ifndef __ADC_H__
#define __ADC_H__


// Initialize this module.
// Can be used any time to reset the module's state.
// Will stop sampling on all pins.
void ADCInit();

// Set a pin to be periodically samples.
// Call will be ignored if pin does not sopprt analog input, or is already set
// for sampling.
void ADCSetScan(int pin);

// Stop sampling a pin.
// Call will be ignored if pin does not support analog input, or is not being
// sampled.
void ADCClrScan(int pin);


#endif  // __ADC_H__
