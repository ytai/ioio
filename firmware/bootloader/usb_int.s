  .section int.sec, code
  .global __USB1InterruptWrapper
__USB1InterruptWrapper:
  mov   _pass_usb_to_app
  bra   z, __USB1Interrupt
  bra   __APP_USB1Interrupt
  return
