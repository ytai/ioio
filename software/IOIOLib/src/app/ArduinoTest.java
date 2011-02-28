package app;

import ioio.util.ArduinoSyntax;

/**
 * Test of using the hack to simulate arduino land.
 * @author arshan
 *
 */
public class ArduinoTest extends ArduinoSyntax {
    
    public void setup() {
        // doesnt do processing yet, but can do scrolling text.
        setupScrollingText();
        pinMode(1, OUTPUT);
        pinMode(2, INPUT);
    }
        
    public void loop() {
        digitalWrite(1, HIGH);
        if (digitalRead(2)) {
            text("Woot!");
        }
        delay(500);
        digitalWrite(1, LOW);
        if (! digitalRead(2)) {
            text("Double Woot!");
        }
        delay(500);
    }
}
