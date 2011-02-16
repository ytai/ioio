package ioio.lib.pic;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

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

    Integer allocateModule() {
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

    public void releaseModule(int moduleId) {
        if (!allocatedModuleIds.contains(moduleId)) {
            throw new IllegalArgumentException("moduleId: " + moduleId+ "; not yet allocated");
        }
        synchronized (availableModuleIds) {
            availableModuleIds.add(moduleId);
            allocatedModuleIds.remove(moduleId);
        }
    }
}
