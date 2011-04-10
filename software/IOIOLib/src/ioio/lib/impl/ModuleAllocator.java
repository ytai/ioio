package ioio.lib.impl;

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

    private Set<Integer> availableModuleIds;
    private Set<Integer> allocatedModuleIds;

    public ModuleAllocator(Collection<Integer> availableModuleIds) {
        this.availableModuleIds = new TreeSet<Integer>(availableModuleIds);
        allocatedModuleIds = new HashSet<Integer>();
    }

    public ModuleAllocator(int maxModules) {
        this(getList(maxModules));
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
        if (availableModuleIds.isEmpty()) {
            return null;
        }
        synchronized (availableModuleIds) {
            Integer moduleId = availableModuleIds.iterator().next();
            availableModuleIds.remove(moduleId);
            allocatedModuleIds.add(moduleId);
            return moduleId;
        }
    }

    /**
     * @param moduleId the moduleId to be released; throws {@link IllegalArgumentException} if
     *     a moduleId is re-returned, or an invalid moduleId is provided
     */
    public void releaseModule(int moduleId) {
        if (!allocatedModuleIds.contains(moduleId)) {
            throw new IllegalArgumentException("moduleId: " + moduleId+ "; not yet allocated");
        }
        synchronized (availableModuleIds) {
            availableModuleIds.add(moduleId);
            allocatedModuleIds.remove(moduleId);
        }
    }

    /**
     * Requests a moduleId to be allocated.
     * @param moduleId the module id to reserve
     * @return true if the module was reserved, false if it wasn't
     */
    public boolean requestAllocate(int moduleId) {
        synchronized (availableModuleIds) {
            if (!availableModuleIds.contains(moduleId)) {
                return false;
            }
            availableModuleIds.remove(moduleId);
            allocatedModuleIds.add(moduleId);
        }
        return true;
    }
}
