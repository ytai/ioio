package ioio.tests.torture;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

class ResourceAllocator {
	static final int PIN_PAIR_DIGITAL = 0x0001; 
	static final int PIN_PAIR_ANALOG = 0x0002; 
	static final int PIN_PAIR_PERIPHERAL = 0x0004; 
	
	private static int[] PIN_PAIR_CAPS = new int[] {
		PIN_PAIR_DIGITAL,						               	   // 1,2
		PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 3,4
		PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 5,6
		PIN_PAIR_DIGITAL,                                          // 7,8
		PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 9,10
		PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 11,12
		PIN_PAIR_DIGITAL | PIN_PAIR_PERIPHERAL,                    // 13,14
		PIN_PAIR_DIGITAL,          			                       // 15,16
		PIN_PAIR_DIGITAL,     	    			               	   // 17,18
		PIN_PAIR_DIGITAL,                                   	   // 19,20
		PIN_PAIR_DIGITAL,     					               	   // 21,22
		PIN_PAIR_DIGITAL,     					               	   // 23,24
		PIN_PAIR_DIGITAL,     						               // 25,26
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
	
	private class PinPair {
		public final int first_pin_num;
		public final int caps;
		public PinPair(int num, int c) {
			first_pin_num = num;
			caps = c;
		}
	}
	
	List<PinPair> pinPairs_ = new LinkedList<ResourceAllocator.PinPair>();
	private int[] peripherals_;
	
	public ResourceAllocator() {
		freeAll();
	}
	
	public synchronized void freeAll() {
		pinPairs_.clear();
		for(int pin = 1; pin <= 47; pin += 2) {
			freePinPair(pin);
		}
		peripherals_ = new int[] { 9, 4, 3, 3, 3 };
	}
	
	enum PeripheralType {
		PWM,
		UART,
		SPI,
		INCAP_SINGLE,
		INCAP_DOUBLE
	}
	
	public synchronized int allocatePinPair(int caps) throws InterruptedException {
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
		pinPairs_.add(new PinPair(pin, PIN_PAIR_CAPS[(pin - 1) / 2]));
		notify();
	}
	
	public synchronized void allocPeripheral(PeripheralType type) throws InterruptedException {
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
