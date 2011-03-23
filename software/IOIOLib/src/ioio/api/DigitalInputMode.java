package ioio.api;

public enum DigitalInputMode {
    // We shouldnt be keeping wire-protocol values here.
    FLOATING(0),
    PULL_UP(1),
    PULL_DOWN(2)
    ;

    private final int bitValue;

    private DigitalInputMode(int bitValue) {
        this.bitValue = bitValue;
    }

    public int getBitValue() {
        return bitValue;
    }
}