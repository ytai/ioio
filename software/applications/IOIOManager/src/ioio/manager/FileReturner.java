package ioio.manager;

import android.app.Activity;

public interface FileReturner {
	public static final String SELECTED_FILE_EXTRA = "ioio.manager.SELECTED_FILE";
	public static final String ERROR_MESSAGE_EXTRA  = "ioio.manager.ERROR_MESSAGE";
	public static final int RESULT_ERROR = Activity.RESULT_FIRST_USER;
}