package ioio.lib.spi;

public class NoRuntimeSupportException extends Exception {
	private static final long serialVersionUID = -6559208663699429514L;

	public NoRuntimeSupportException(String desc) {
		super(desc);
	}
}