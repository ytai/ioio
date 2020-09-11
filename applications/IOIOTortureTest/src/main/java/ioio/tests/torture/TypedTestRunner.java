package ioio.tests.torture;

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.spi.Log;

class TypedTestRunner<E> implements TestRunner {
    Test<E> test_;
    TestResultAggregator<E> agg_;

    TypedTestRunner(Test<E> test, TestResultAggregator<E> agg) {
        test_ = test;
        agg_ = agg;
    }

    @Override
    public void run() throws ConnectionLostException, InterruptedException {
        try {
            agg_.addResult(test_.run());
        } catch (InterruptedException e) {
            agg_.addException(e);
            throw e;
        } catch (ConnectionLostException e) {
            throw e;
        } catch (Exception e) {
            Log.e("TortureTest", "Test threw an exception: ", e);
            agg_.addException(e);
        }
    }

    @Override
    public String testClassName() {
        return test_.getClass().getName();
    }
}
