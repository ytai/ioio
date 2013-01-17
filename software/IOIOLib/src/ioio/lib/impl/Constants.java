/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
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
package ioio.lib.impl;

class Constants {
	static final int NUM_PINS = 49;
	static final int NUM_ANALOG_PINS = 16;
	static final int NUM_PWM_MODULES = 9;
	static final int NUM_UART_MODULES = 4;
	static final int NUM_SPI_MODULES = 3;
	static final int NUM_TWI_MODULES = 3;
	static final int[] INCAP_MODULES_DOUBLE = new int[] { 0, 2, 4};
	static final int[] INCAP_MODULES_SINGLE = new int[] { 6, 7, 8};
	static final int BUFFER_SIZE = 1024;
	static final int PACKET_BUFFER_SIZE = 256;
	
	static final int[][] TWI_PINS = new int[][] {{ 4, 5 }, { 47, 48 }, { 26, 25 }};
	static final int[] ICSP_PINS = new int[] { 36, 37, 38 };
	static final int[] RGB_LED_MATRIX_PINS = new int[] { 7, 10, 11, 19, 20, 21, 22, 23, 24, 25, 27, 28};
}
