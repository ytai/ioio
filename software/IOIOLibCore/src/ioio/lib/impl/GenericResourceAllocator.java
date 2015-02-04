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

import ioio.lib.api.exception.OutOfResourceException;
import ioio.lib.impl.ResourceManager.Resource;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

class GenericResourceAllocator implements ResourceManager.ResourceAllocator {
	private final List<Integer> available_;
	private final Set<Integer> allocated_;

	public GenericResourceAllocator(int offset, int count) {
		available_ = new ArrayList<Integer>(count);
		allocated_ = new HashSet<Integer>(count);
		for (int i = 0; i < count; i++) {
			available_.add(i + offset);
		}
	}

	public GenericResourceAllocator(int ids[]) {
		available_ = new ArrayList<Integer>(ids.length);
		allocated_ = new HashSet<Integer>(ids.length);
		for (int i = 0; i < ids.length; i++) {
			available_.add(ids[i]);
		}
	}

	@Override
	public synchronized void alloc(Resource r) {
		if (available_.isEmpty()) {
			throw new OutOfResourceException(
					"No more resources of the requested type: " + r.type);
		}
		r.id = available_.remove(available_.size() - 1);
		allocated_.add(r.id);
	}

	@Override
	public synchronized void free(Resource r) {
		if (!allocated_.contains(r.id)) {
			throw new IllegalArgumentException("Resource " + r
					+ " not yet allocated");
		}
		available_.add(r.id);
		allocated_.remove(r.id);
	}
}
