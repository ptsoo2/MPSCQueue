#pragma once

/*
*	_mm_pause						: https://yonmy.com/archives/99 (대기 속도를 늦춤으로써 파이프라인의 더 적은 부분을 사용)// 겁나작은 sleep 이라 생각하면 된다. (us(마이크로세컨즈) 단위)
* 	(리눅스에서의 사용)				: https://stackoverflow.com/questions/7371869/minimum-time-a-thread-can-pause-in-linux
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
			// memory_order_acquire : 모든 메모리 명령들이 이 시점 이전으로 재배치 되는 것을 금지한다.
			// test_and_set			: 비교 후 true(1) 로 전환
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
			// flag_ = false; 와 같다.
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
			// 아.. gcc 에선 못쓰는듯..?

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
