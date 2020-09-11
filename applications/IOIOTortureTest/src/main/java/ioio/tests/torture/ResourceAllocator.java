package ioio.tests.torture;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

class ResourceAllocator {
	enum Board {
		SPRK0015(Hardware.IOIO0002),
		SPRK0016(Hardware.IOIO0003),
		MINT0010(Hardware.IOIO0003),
		SPRK0020(Hardware.IOIO0004);

		public final Hardware hardware;

		Board(Hardware hw) {
			hardware = hw;
		}

		private static class Hardware {
			private static final int[] MAP_IOIO0002_IOIO0003 = {
				PIN_PAIR_DIGITAL,                                          // 1,2
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 3,4
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 5,6
				PIN_PAIR_DIGITAL,                                          // 7,8
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 9,10
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 11,12
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 13,14
				PIN_PAIR_DIGITAL,                                          // 15,16
				PIN_PAIR_DIGITAL,                                          // 17,18
				PIN_PAIR_DIGITAL,                                          // 19,20
				PIN_PAIR_DIGITAL,                                          // 21,22
				PIN_PAIR_DIGITAL,                                          // 23,24
				PIN_PAIR_DIGITAL,                                          // 25,26
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 27,28
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 29,30
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 31,32
				PIN_PAIR_DIGITAL                       | PIN_PAIR_ANALOG,  // 33,34
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 35,36
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 37,38
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 39,40
				PIN_PAIR_DIGITAL                       | PIN_PAIR_ANALOG,  // 41,42
				PIN_PAIR_DIGITAL                       | PIN_PAIR_ANALOG,  // 43,44
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 45,46
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL                     // 47,48
			};

			private static final int[] MAP_IOIO0004 = {
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 1,2
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 3,4
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 5,6
				PIN_PAIR_DIGITAL,                                          // 7,8
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 9,10
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 11,12
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 13,14
				PIN_PAIR_DIGITAL,                                          // 15,16
				PIN_PAIR_DIGITAL,                                          // 17,18
				PIN_PAIR_DIGITAL,                                          // 19,20
				PIN_PAIR_DIGITAL,                                          // 21,22
				PIN_PAIR_DIGITAL,                                          // 23,24
				PIN_PAIR_DIGITAL,                                          // 25,26
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 27,28
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 29,30
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 31,32
				PIN_PAIR_DIGITAL                       | PIN_PAIR_ANALOG,  // 33,34
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 35,36
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 37,38
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG,  // 39,40
				PIN_PAIR_DIGITAL                       | PIN_PAIR_ANALOG,  // 41,42
				PIN_PAIR_DIGITAL                       | PIN_PAIR_ANALOG,  // 43,44
				PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL | PIN_PAIR_ANALOG   // 45,46
			};

			private static final Hardware IOIO0002 = new Hardware(MAP_IOIO0002_IOIO0003);
			private static final Hardware IOIO0003 = IOIO0002;
			private static final Hardware IOIO0004 = new Hardware(MAP_IOIO0004);

			public final int[] pinCaps_;

			private Hardware(int[] caps) {
				pinCaps_ = caps;
			}

		}
	}

	static final int PIN_PAIR_DIGITAL = 0x0001;
	static final int PIN_PAIR_ANALOG = 0x0002;
	static final int PIN_PAIR_PERIPHERAL = 0x0004;

	private class PinPair {
		public final int first_pin_num;
		public final int caps;

		public PinPair(int num, int c) {
			first_pin_num = num;
			caps = c;
		}
	}

	private final int[] pinCaps_;
	List<PinPair> pinPairs_ = new LinkedList<ResourceAllocator.PinPair>();
	private int[] peripherals_;

	public ResourceAllocator(Board board) {
		pinCaps_ = board.hardware.pinCaps_;
		freeAll();
	}
	
	public synchronized void freeAll() {
		pinPairs_.clear();
		for(int i = 0; i < pinCaps_.length; ++i) {
//			Uncomment the condition if running while a programmer is on pins
//			37, 38
//			if (i != 18) {
				freePinPair(i * 2 + 1);
//			}
		}
		peripherals_ = new int[] { 9, 4, 3, 3, 3 };
	}

	enum PeripheralType {
		PWM, UART, SPI, INCAP_SINGLE, INCAP_DOUBLE
	}

	public synchronized int allocatePinPair(int caps)
			throws InterruptedException {
		while (true) {
			PinPair p;
			for (Iterator<PinPair> i = pinPairs_.iterator(); i.hasNext();) {
				p = i.next();
				if ((p.caps & caps) != 0) {
					i.remove();
					return p.first_pin_num;
				}
			}
			wait();
		}
	}

	public synchronized void freePinPair(int pin) {
		pinPairs_.add(new PinPair(pin, pinCaps_[(pin - 1) / 2]));
		notify();
	}

	public synchronized void allocPeripheral(PeripheralType type)
			throws InterruptedException {
		while (peripherals_[type.ordinal()] == 0) {
			wait();
		}
		peripherals_[type.ordinal()]--;
	}

	public synchronized void freePeripheral(PeripheralType type) {
		peripherals_[type.ordinal()]++;
		notify();
	}
}
