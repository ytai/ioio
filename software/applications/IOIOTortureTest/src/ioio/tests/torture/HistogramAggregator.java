package ioio.tests.torture;

import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;
import java.util.SortedMap;
import java.util.TreeMap;

import android.app.Activity;
import android.widget.TextView;

class HistogramAggregator implements TestResultAggregator<List<Float>> {
	SortedMap<Float, Integer> map_ = new TreeMap<Float, Integer>();
	int total_;
	final Activity activity_;
	final TextView[] percentiles_;
	final TextView totalView_;

	public HistogramAggregator(Activity activity, TextView[] percentiles, TextView total) {
		activity_ = activity;
		percentiles_ = percentiles.clone();
		totalView_ = total;
		clear();
	}

	@Override
	public synchronized void clear() {
		map_.clear();
		total_ = 0;
		updateViews();
	}

	@Override
	public synchronized void addResult(List<Float> result) {
		for (Float f: result) {
			Integer value = map_.get(f);
			if (value == null) {
				map_.put(f, 1);
			} else {
				value++;
			}
		}
		total_ += result.size();
		updateViews();
	}

	private void updateViews() {
		int current;
		Iterator<Entry<Float, Integer>> iterator = map_.entrySet().iterator();
		if (iterator.hasNext()) {
			Entry<Float, Integer> next = iterator.next();
			current = next.getValue();
			for (int i = 0; i < percentiles_.length; ++i) {
				final int limit = total_ * i / (percentiles_.length - 1);
				while (current < limit && iterator.hasNext()) {
					next = iterator.next();
					current += next.getValue();
				}
				setText(percentiles_[i], String.valueOf(Math.round(next.getKey())));
			}
		} else {
			for (int i = 0; i < percentiles_.length; ++i) {
				setText(percentiles_[i], "-");
			}
		}
		setText(totalView_, String.valueOf(total_));
	}
	
	private void setText(final TextView tv, final String text) {
		activity_.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				tv.setText(text);
			}
		});
	}
}
