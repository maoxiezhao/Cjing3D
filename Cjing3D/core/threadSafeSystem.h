#pragma once

#include "common\common.h"
#include "utils\thread\spinLock.h"


namespace Cjing3D
{

// �̰߳�ȫϵͳ������ϵͳ�Ӹ�������
class ThreadSafeSystem
{
public:
	ThreadSafeSystem();
	~ThreadSafeSystem();

	void Lock();
	void UnLock();

private:
	SpinLock mSpinLock;
};

}