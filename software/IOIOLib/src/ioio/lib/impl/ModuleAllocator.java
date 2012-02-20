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

import ioio.lib.api.exception.OutOfResourceException;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

/**
 * Utility to allocate and assign unique module ids.
 * A module id is requested via {@link #allocateModule()}
 * and released via {@link #releaseModule(int)}.
 *
 * @author birmiwal
 */
class ModuleAllocator {
    private final Set<Integer> availableModuleIds_;
    private final Set<Integer> allocatedModuleIds_;
	private final String name_;

    public ModuleAllocator(Collection<Integer> availableModuleIds, String name) {
        this.availableModuleIds_ = new TreeSet<Integer>(availableModuleIds);
        allocatedModuleIds_ = new HashSet<Integer>();
        name_ = name;
    }

    public ModuleAllocator(int[] availableModuleIds, String name) {
    	this(getList(availableModuleIds), name);
    }

    public ModuleAllocator(int maxModules, String name) {
        this(getList(maxModules), name);
    }

    private static Collection<Integer> getList(int maxModules) {
        List<Integer> availableModuleIds = new ArrayList<Integer>();
        for (int i = 0; i < maxModules; i++) {
            availableModuleIds.add(i);
        }
        return availableModuleIds;
    }

    private static Collection<Integer> getList(int[] array) {
        List<Integer> availableModuleIds = new ArrayList<Integer>(array.length);
        for (int i : array) {
            availableModuleIds.add(i);
        }
        return availableModuleIds;
    }

    /**
     * @return a module id that was allocated, or {@code null} if nothing was available
     */
    public synchronized Integer allocateModule() {
        if (availableModuleIds_.isEmpty()) {
        	throw new OutOfResourceException("No more resources of the requested type: " + name_);
        }
        Integer moduleId = availableModuleIds_.iterator().next();
        availableModuleIds_.remove(moduleId);
        allocatedModuleIds_.add(moduleId);
        return moduleId;
    }

    /**
     * @param moduleId the moduleId to be released; throws {@link IllegalArgumentException} if
     *     a moduleId is re-returned, or an invalid moduleId is provided
     */
    public synchronized void releaseModule(int moduleId) {
        if (!allocatedModuleIds_.contains(moduleId)) {
            throw new IllegalArgumentException("moduleId: " + moduleId+ "; not yet allocated");
        }
        availableModuleIds_.add(moduleId);
        allocatedModuleIds_.remove(moduleId);
    }
}
