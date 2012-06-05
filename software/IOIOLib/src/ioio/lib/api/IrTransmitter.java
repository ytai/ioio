/*
 * Copyright 2012 Markus Lanthaler <mail@markus-lanthaler.com>. All rights reserved.
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
package ioio.lib.api;

import ioio.lib.api.exception.ConnectionLostException;

/**
 * A pin used for sending infrared codes.
 * <p>
 * An infrared transmitter pin can be used to send infrared signals.
 * IrTransmitter instances are obtained by calling
 * {@link IOIO#openIrOutput(int pin)}.
 * <p>
 * The infrared code is send by calling {@link #transmitIrCommand(int[]}. The
 * data must encoded as
 * {@link http://www.remotecentral.com/features/irdisp2.htm Pronto} burst-pair
 * stream, i.e., two bytes are combined into one integer and form a burst-pair.
 * <p>
 * The instance is alive since its creation. If the connection with the IOIO
 * drops at any point, the instance transitions to a disconnected state, in
 * which every attempt to use the pin (except {@link #close()}) will throw a
 * {@link ConnectionLostException}. Whenever {@link #close()} is invoked the
 * instance may no longer be used. Any resources associated with it are freed
 * and can be reused.
 * <p>
 * Typical usage:
 *
 * <pre>
 * IrTransmitter ir = ioio.openIrOutput(10);  // IR LED anode on pin 10.
 * ir.transmitIrCommand(prontoCode);  // send Pronto encoded IR signal.
 * ...
 * ir.close();  // pin 10 can now be used for something else.
 * </pre>
 *
 * @author Markus Lanthaler <mail@markus-lanthaler.com>
 * @author Andrej Eisfeld <andrej.eisfeld@hs-furtwangen.de>
 * @author Sergej Proskurin <sergej.proskurin@googlemail.com>
 */
public interface IrTransmitter extends Closeable {
    /**
     * Transmit a Pronto-encoded IR command.
     *
     * @param data
     *            The data to transmit.
     * @throws ConnectionLostException
     *             The connection with the IOIO has been lost.
     */
    public void transmitIrCommand(int[] data) throws ConnectionLostException;
}
