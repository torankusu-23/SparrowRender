#pragma once
#include <atomic>

class SpinLock {
public:
	SpinLock() : flag_(false)
	{}

	void lock()
	{
		bool expect = false;

		//���flag��expect��ֵ��������򷵻�true���Ұ�flag��Ϊtrue�� 
		//��������򷵻�false���Ұ�expect��Ϊtrue(����ÿ��ѭ��һ��Ҫ��expect��ԭ)
		while (!flag_.compare_exchange_weak(expect, true))
		{
			//����һ��Ҫ��expect��ԭ; ִ��ʧ��ʱexpect�����δ����
			expect = false;
		}
	}

	void unlock()
	{
		flag_.store(false);
	}

private:
	std::atomic<bool> flag_;
};