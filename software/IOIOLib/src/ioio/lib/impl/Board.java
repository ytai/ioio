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

enum Board {
	SPRK0015(Hardware.IOIO0002),
	SPRK0016(Hardware.IOIO0003),
	MINT0010(Hardware.IOIO0003),
	SPRK0020(Hardware.IOIO0004);

	public final Hardware hardware;

	private Board(Hardware hw) {
		hardware = hw;
	}

	static class Hardware {
		private static final boolean[][] MAP_IOIO0002_IOIO0003 = {
			// p. out	p. in	analog
			{ true,		true,	false },  // 0
			{ false,	false,	false },  // 1
			{ false,	false,	false },  // 2
			{ true,		true,	false },  // 3
			{ true,		true,	false },  // 4
			{ true,		true,	false },  // 5
			{ true,		true,	false },  // 6
			{ true,		true,	false },  // 7
			{ false,	false,	false },  // 8
			{ false,	true,	false },  // 9
			{ true,		true,	false },  // 10
			{ true,		true,	false },  // 11
			{ true,		true,	false },  // 12
			{ true,		true,	false },  // 13
			{ true,		true,	false },  // 14
			{ false,	false,	false },  // 15
			{ false,	false,	false },  // 16
			{ false,	false,	false },  // 17
			{ false,	false,	false },  // 18
			{ false,	false,	false },  // 19
			{ false,	false,	false },  // 20
			{ false,	false,	false },  // 21
			{ false,	false,	false },  // 22
			{ false,	false,	false },  // 23
			{ false,	false,	false },  // 24
			{ false,	false,	false },  // 25
			{ false,	false,	false },  // 26
			{ true,		true,	false },  // 27
			{ true,		true,	false },  // 28
			{ true,		true,	false },  // 29
			{ true,		true,	false },  // 30
			{ true,		true,	true  },  // 31
			{ true,		true,	true  },  // 32
			{ false,	false,	true  },  // 33
			{ true,		true,	true  },  // 34
			{ true,		true,	true  },  // 35
			{ true,		true,	true  },  // 36
			{ true,		true,	true  },  // 37
			{ true,		true,	true  },  // 38
			{ true,		true,	true  },  // 39
			{ true,		true,	true  },  // 40
			{ false,	false,	true  },  // 41
			{ false,	false,	true  },  // 42
			{ false,	false,	true  },  // 43
			{ false,	false,	true  },  // 44
			{ true,		true,	true  },  // 45
			{ true,		true,	true  },  // 46
			{ true,		true,	false },  // 47
			{ true,		true,	false }   // 48
		};
		private static final boolean[][] MAP_IOIO0004 = {
			// p. out	p. in	analog
			{ false,	false,	false },  // 0
			{ true,		true,	false },  // 1
			{ true,		true,	false },  // 2
			{ true,		true,	false },  // 3
			{ true,		true,	false },  // 4
			{ true,		true,	false },  // 5
			{ true,		true,	false },  // 6
			{ true,		true,	false },  // 7
			{ false,	false,	false },  // 8
			{ false,	true,	false },  // 9
			{ true,		true,	false },  // 10
			{ true,		true,	false },  // 11
			{ true,		true,	false },  // 12
			{ true,		true,	false },  // 13
			{ true,		true,	false },  // 14
			{ false,	false,	false },  // 15
			{ false,	false,	false },  // 16
			{ false,	false,	false },  // 17
			{ false,	false,	false },  // 18
			{ false,	false,	false },  // 19
			{ false,	false,	false },  // 20
			{ false,	false,	false },  // 21
			{ false,	false,	false },  // 22
			{ false,	false,	false },  // 23
			{ false,	false,	false },  // 24
			{ false,	false,	false },  // 25
			{ false,	false,	false },  // 26
			{ true,		true,	false },  // 27
			{ true,		true,	false },  // 28
			{ true,		true,	false },  // 29
			{ true,		true,	false },  // 30
			{ true,		true,	true  },  // 31
			{ true,		true,	true  },  // 32
			{ false,	false,	true  },  // 33
			{ true,		true,	true  },  // 34
			{ true,		true,	true  },  // 35
			{ true,		true,	true  },  // 36
			{ true,		true,	true  },  // 37
			{ true,		true,	true  },  // 38
			{ true,		true,	true  },  // 39
			{ true,		true,	true  },  // 40
			{ false,	false,	true  },  // 41
			{ false,	false,	true  },  // 42
			{ false,	false,	true  },  // 43
			{ false,	false,	true  },  // 44
			{ true,		true,	true  },  // 45
			{ true,		true,	true  }   // 46
		};
		static final Hardware IOIO0002 = new Hardware(MAP_IOIO0002_IOIO0003,
				9, 4, 3, new int[] {0, 2, 4}, new int[] { 6, 7, 8 },
				new int[][] {{ 4, 5 }, { 47, 48 }, { 26, 25 }},
				new int[] { 36, 37, 38 });
		static final Hardware IOIO0003 = IOIO0002;
		static final Hardware IOIO0004 = new Hardware(MAP_IOIO0004,
				9, 4, 3, new int[] {0, 2, 4}, new int[] { 6, 7, 8 },
				new int[][] {{ 4, 5 }, { 1, 2 }, { 26, 25 }},
				new int[] { 36, 37, 38 });

		private enum Function {
			PERIPHERAL_OUT,
			PERIPHERAL_IN,
			ANALOG_IN
		}

		private final boolean[][] map_;
		private final int numPwmModules_;
		private final int numUartModules_;
		private final int numSpiModules_;
		private final int[] incapSingleModules_;
		private final int[] incapDoubleModules_;
		private final int[][] twiPins_;
		private final int[] icspPins_;

		private Hardware(boolean[][] map, int numPwmModules,
				int numUartModules, int numSpiModules,
				int[] incapDoubleModules, int[] incapSingleModules,
				int[][] twiPins, int[] icspPins) {
			if (map == null)
				throw new IllegalArgumentException("WTF");
			map_ = map;
			numPwmModules_ = numPwmModules;
			numUartModules_ = numUartModules;
			numSpiModules_ = numSpiModules;
			incapSingleModules_ = incapSingleModules;
			incapDoubleModules_ = incapDoubleModules;
			twiPins_ = twiPins;
			icspPins_ = icspPins;
		}

		int numPins() {
			return map_.length;
		}

		int numAnalogPins() {
			int result = 0;
			for (boolean[] b : map_) {
				if (b[Function.ANALOG_IN.ordinal()]) {
					++result;
				}
			}
			return result;
		}

		int numPwmModules() {
			return numPwmModules_;
		}

		int numUartModules() {
			return numUartModules_;
		}

		int numSpiModules() {
			return numSpiModules_;
		}

		int numTwiModules() {
			return twiPins().length;
		}

		int[] incapSingleModules() {
			return incapSingleModules_;
		}

		int[] incapDoubleModules() {
			return incapDoubleModules_;
		}

		int[][] twiPins() {
			return twiPins_;
		}

		int[] icspPins() {
			return icspPins_;
		}

		void checkSupportsAnalogInput(int pin) {
			checkValidPin(pin);
			if (!map_[pin][Function.ANALOG_IN.ordinal()]) {
				throw new IllegalArgumentException("Pin " + pin
						+ " does not support analog input");
			}
		}

		void checkSupportsPeripheralInput(int pin) {
			checkValidPin(pin);
			if (!map_[pin][Function.PERIPHERAL_IN.ordinal()]) {
				throw new IllegalArgumentException("Pin " + pin
						+ " does not support peripheral input");
			}
		}

		void checkSupportsPeripheralOutput(int pin) {
			checkValidPin(pin);
			if (!map_[pin][Function.PERIPHERAL_OUT.ordinal()]) {
				throw new IllegalArgumentException("Pin " + pin
						+ " does not support peripheral output");
			}
		}

		void checkValidPin(int pin) {
			if (pin < 0 || pin >= map_.length) {
				throw new IllegalArgumentException("Illegal pin: " + pin);
			}
		}

		void checkSupportsCapSense(int pin) {
			checkValidPin(pin);
			// Currently, all analog pins are also cap-sense.
			if (!map_[pin][Function.ANALOG_IN.ordinal()]) {
				throw new IllegalArgumentException("Pin " + pin
						+ " does not support cap-sense");
			}
		}
	}
}
