/*
 * Copyright 2013 Ytai Ben-Tsvi. All rights reserved.
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

import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;

public class ResourceManager {
	private ResourceAllocator[] allocators_ = new ResourceAllocator[ResourceType
			.values().length];

	enum ResourceType {
		PIN, OUTCOMPARE, INCAP_SINGLE, INCAP_DOUBLE, TWI, ICSP, UART, SPI, SEQUENCER
	}

	public static class Resource {
		public final ResourceType type;
		public int id;

		Resource(ResourceType t) {
			type = t;
			id = -1;
		}

		Resource(ResourceType t, int i) {
			type = t;
			id = i;
		}

		@Override
		public String toString() {
			if (id == -1) {
				return type.toString();
			} else {
				return type.toString() + "(" + id + ")";
			}
		}
	}

	public ResourceManager(Board.Hardware hardware) {
		allocators_[ResourceType.PIN.ordinal()] = new SpecificResourceAllocator(
				0, hardware.numPins());
		allocators_[ResourceType.TWI.ordinal()] = new SpecificResourceAllocator(
				0, hardware.numTwiModules());
		allocators_[ResourceType.ICSP.ordinal()] = new GenericResourceAllocator(
				0, 1);
		allocators_[ResourceType.OUTCOMPARE.ordinal()] = new GenericResourceAllocator(
				0, hardware.numPwmModules());
		allocators_[ResourceType.UART.ordinal()] = new GenericResourceAllocator(
				0, hardware.numUartModules());
		allocators_[ResourceType.SPI.ordinal()] = new GenericResourceAllocator(
				0, hardware.numSpiModules());
		allocators_[ResourceType.INCAP_SINGLE.ordinal()] = new GenericResourceAllocator(
				hardware.incapSingleModules());
		allocators_[ResourceType.INCAP_DOUBLE.ordinal()] = new GenericResourceAllocator(
				hardware.incapDoubleModules());
		allocators_[ResourceType.SEQUENCER.ordinal()] = new GenericResourceAllocator(
				0, 1);
	}

	@SuppressWarnings("unchecked")
	public synchronized void alloc(Object... args) {
		int i = 0;
		try {
			for (; i < args.length; ++i) {
				if (args[i] != null) {
					if (args[i] instanceof Resource) {
						alloc((Resource) args[i]);
					} else if (args[i] instanceof Resource[]) {
						alloc(Arrays.asList((Resource[]) args[i]));
					} else if (args[i] instanceof Collection<?>) {
						alloc((Collection<Resource>) args[i]);
					} else {
						throw new IllegalArgumentException();
					}
				}
			}
		} catch (RuntimeException e) {
			for (int j = 0; j < i; ++j) {
				if (args[j] instanceof Resource) {
					free((Resource) args[j]);
				} else if (args[j] instanceof Resource[]) {
					free(Arrays.asList((Resource[]) args[j]));
				} else if (args[i] instanceof Collection<?>) {
					free((Collection<Resource>) args[i]);
				}
			}
			throw e;
		}
	}

	@SuppressWarnings("unchecked")
	public synchronized void free(Object... args) {
		for (int i = 0; i < args.length; ++i) {
			if (args[i] instanceof Resource) {
				free((Resource) args[i]);
			} else if (args[i] instanceof Resource[]) {
				free(Arrays.asList((Resource[]) args[i]));
			} else if (args[i] instanceof Collection<?>) {
				free((Collection<Resource>) args[i]);
			} else {
				throw new IllegalArgumentException();
			}
		}
	}

	public synchronized void alloc(Collection<Resource> resources) {
		int i = 0;
		try {
			Iterator<Resource> iter = resources.iterator();
			while (iter.hasNext()) {
				alloc(iter.next());
				++i;
			}
		} catch (RuntimeException e) {
			Iterator<Resource> iter = resources.iterator();
			while (i-- > 0) {
				free(iter.next());
			}
			throw e;
		}
	}

	public synchronized void free(Collection<Resource> resources) {
		for (Resource r : resources) {
			free(r);
		}
	}

	public synchronized void alloc(Resource r) {
		if (r != null) {
			allocators_[r.type.ordinal()].alloc(r);
		}
	}

	public synchronized void free(Resource r) {
		if (r != null) {
			allocators_[r.type.ordinal()].free(r);
		}
	}

	static interface ResourceAllocator {
		public void alloc(Resource r);

		public void free(Resource r);
	}
}
