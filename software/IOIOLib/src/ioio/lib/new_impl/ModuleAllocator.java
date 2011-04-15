package ioio.lib.new_impl;

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
public class ModuleAllocator {

    private Set<Integer> availableModuleIds_;
    private Set<Integer> allocatedModuleIds_;
	private String name_;

    public ModuleAllocator(Collection<Integer> availableModuleIds, String name) {
        this.availableModuleIds_ = new TreeSet<Integer>(availableModuleIds);
        allocatedModuleIds_ = new HashSet<Integer>();
        name_ = name;
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

    /**
     * @return a module id that was allocated, or {@code null} if nothing was available
     */
    public Integer allocateModule() {
        if (availableModuleIds_.isEmpty()) {
        	throw new OutOfResourceException("No more resources of the requested type: " + name_);
        }
        synchronized (availableModuleIds_) {
            Integer moduleId = availableModuleIds_.iterator().next();
            availableModuleIds_.remove(moduleId);
            allocatedModuleIds_.add(moduleId);
            return moduleId;
        }
    }

    /**
     * @param moduleId the moduleId to be released; throws {@link IllegalArgumentException} if
     *     a moduleId is re-returned, or an invalid moduleId is provided
     */
    public void releaseModule(int moduleId) {
        if (!allocatedModuleIds_.contains(moduleId)) {
            throw new IllegalArgumentException("moduleId: " + moduleId+ "; not yet allocated");
        }
        synchronized (availableModuleIds_) {
            availableModuleIds_.add(moduleId);
            allocatedModuleIds_.remove(moduleId);
        }
    }
}
