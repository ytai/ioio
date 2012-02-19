package ioio.tests.torture;

import ioio.lib.api.exception.ConnectionLostException;

class TypedTestRunner<E> implements TestRunner {
	Test<E> test_;
	TestResultAggregator<E> agg_;
	
	TypedTestRunner(Test<E> test, TestResultAggregator<E> agg) {
		test_ = test;
		agg_ = agg;
	}
	
	@Override
	public void run() throws ConnectionLostException, InterruptedException {
		agg_.addResult(test_.run());
	}

	@Override
	public String testClassName() {
		return test_.getClass().getName();
	}
}
