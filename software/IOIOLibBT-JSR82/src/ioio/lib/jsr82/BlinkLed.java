package ioio.lib.jsr82;


import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.PwmOutput;
import ioio.lib.util.IOIOConnectionRegistry;

public class BlinkLed {
  public static void main(String[] args) throws Exception {
    IOIOConnectionRegistry.addBootstraps(JSR82BluetoothIOIOConnectionBootstrap.class.getCanonicalName());
    IOIO ioio = IOIOFactory.create();
    ioio.waitForConnect();
    
    PwmOutput pwm = ioio.openPwmOutput(IOIO.LED_PIN, 200);
    while (true) {
      pwm.setDutyCycle(0.5f + 0.5f*(float)Math.sin(2*Math.PI * (System.currentTimeMillis() % 1000) / 1000f));
    }
  }
}