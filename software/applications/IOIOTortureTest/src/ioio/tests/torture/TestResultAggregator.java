package ioio.tests.torture;

interface TestResultAggregator<E> {
	void clear();
	void addResult(E result); 
}
