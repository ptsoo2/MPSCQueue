#pragma once

/*
*	_mm_pause						: https://yonmy.com/archives/99 (��� �ӵ��� �������ν� ������������ �� ���� �κ��� ���)// �̳����� sleep �̶� �����ϸ� �ȴ�. (us(����ũ�μ�����) ����)
* 	(������������ ���)				: https://stackoverflow.com/questions/7371869/minimum-time-a-thread-can-pause-in-linux
*/

#include <atomic>
#include <thread>

namespace TSUtil
{
	class CSpinLock
	{
		enum
		{
			MAX_SPIN_COUNT = 1 << 14, // 128
		};

	public:
		CSpinLock() = default;
		~CSpinLock() = default;

	public:
		bool try_lock()
		{
			// memory_order_acquire : ��� �޸� ��ɵ��� �� ���� �������� ���ġ �Ǵ� ���� �����Ѵ�.
			// test_and_set			: �� �� true(1) �� ��ȯ
			return flag_.test_and_set(std::memory_order_acquire) == false;
		}

		void lock()
		{
			// try case - 1
			if (_try_lock(_pause) == true)
				return;

			// try case - 2
			if (_try_lock(std::this_thread::yield) == true)
				return;

			// try case - 3
			while (_try_lock(_sleep) == false);
		}

		void unlock()
		{
			// flag_ = false; �� ����.
			flag_.clear();
		}

	private:
		template<typename TSleep>
		bool _try_lock(TSleep&& sleep)
		{
			size_t tryCount = 0;

			while (try_lock() == false)
			{
				if (tryCount >= MAX_SPIN_COUNT)
					return false;

				++tryCount;
				sleep();
			}

			return true;
		}

		inline static void _sleep()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		inline static void _pause()
		{
			// ��.. gcc ���� �����µ�..?

#ifdef _MSC_VER
			_mm_pause();
#else // _MSC_VER
			__asm volatile ("pause" ::: "memory");
#endif // _MSC_VER
		}

	private:
		std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
	};
}
