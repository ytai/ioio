package ioio.tests.torture;

import android.app.Activity;
import android.widget.TextView;

public class PassFailTestAggregator implements TestResultAggregator<Boolean> {
	private int numPassed_ = 0;
	private int numFailed_ = 0;
	private final Activity activity_;
	private final TextView passTextView_;
	private final TextView failTextView_;
	private final TextView countTextView_;

	PassFailTestAggregator(Activity activity, TextView pass, TextView fail,
			TextView count) {
		activity_ = activity;
		passTextView_ = pass;
		failTextView_ = fail;
		countTextView_ = count;
		clear();
	}

	@Override
	public synchronized void clear() {
		numPassed_ = 0;
		numFailed_ = 0;
		updateViews();
	}

	@Override
	public synchronized void addResult(Boolean result) {
		if (result) {
			numPassed_++;
		} else {
			numFailed_++;
		}
		updateViews();
	}

	private void updateViews() {
		activity_.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				passTextView_.setText(String.valueOf(numPassed_));
				failTextView_.setText(String.valueOf(numFailed_));
				countTextView_.setText(String.valueOf(numFailed_ + numPassed_));
			}
		});
	}
}
