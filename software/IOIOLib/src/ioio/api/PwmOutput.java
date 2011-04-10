/*
 * Copyright 2011. All rights reserved.
 *  
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */
package ioio.api;


import ioio.api.exception.ConnectionLostException;
import ioio.api.exception.InvalidStateException;

import java.io.Closeable;

/**
 * An interface to control the PWM output on a IOIO board.
 *
 * Typical usage: (Simplified, no exception handling)
 * PwmOutput pwmOutput = ioio.openPwmOutput(...);
 * pwmOutput.setPulseWidth(15000);
 * // Use pwm for a while...
 * pwmOutput.close();  // release the resource.
 *
 * @author birmiwal
 */
public interface PwmOutput extends Closeable {
    /**
     * Sets the duty cycle of the output of the PWM.
     *
     * This is identical of calling setPulseWidth with (pulseWidthUs = 1000000*dutyCycle/freqHz)
     *
     * @param dutyCycle valid values from 0.0 to 1.0.
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidStateException In case the pin has been closed.
     */
    public void setDutyCycle(float dutyCycle) throws ConnectionLostException, InvalidStateException;

    /**
     * Sets the width of the pulse (high) in micro seconds.
     *
     * This is identical of calling setDutyCycle with (dutyCycle = pulseWidthUs*freqHz/1000000)
     * If the pulseWidthUs is greater than the cycle-time, then an IllegalArgumentException is thrown.
     *
     * @param pulseWidthUs time in micro seconds.
     * 
     * @throws ConnectionLostException in case connection was lost before running this method.
     * @throws InvalidStateException In case the pin has been closed.
     */
    public void setPulseWidth(int pulseWidthUs) throws ConnectionLostException, InvalidStateException;
}
