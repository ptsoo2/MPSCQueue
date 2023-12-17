#pragma once

#include <array>
#include <queue>
#include <functional>
#include <mutex>

#include "SpinLock.h"

namespace TSUtil
{
	// multiple producer single consumer queue
	template <typename TElement>
	class CMPSCQueue
	{
		enum
		{
			READ = 0,
			WRITE,
			MAX,
		};

		using	queue_t = std::queue<TElement>;
		using	lstQueue_t = std::array<queue_t, MAX>;
		using	lock_t = CSpinLock;

	public:
		CMPSCQueue()
			: readQueue_(&lstQueue_[READ])
			, writeQueue_(&lstQueue_[WRITE])
		{}
		~CMPSCQueue() = default;

	public:
		void push(const TElement& element)
		{
			std::lock_guard<lock_t> grab(lock_);
			writeQueue_->push(element);
		}

		void emplace(TElement&& element)
		{
			std::lock_guard<lock_t> grab(lock_);
			writeQueue_->emplace(std::forward<TElement>(element));
		}

		bool			swap()
		{
			// ������ �ʿ䰡 �ִ� ��츸 ����
			if (readQueue_->empty() == true)
			{
				std::lock_guard<lock_t> grab(lock_);
				std::swap(readQueue_, writeQueue_);
			}

			// ó���Ұ� �ִ���
			return readQueue_->empty() == false;
		}

		void			pop()
		{
			readQueue_->pop();

			// shrink_to_fit
			if (readQueue_->empty() == true)
				readQueue_->swap(queue_t());
		}

		void			flush(std::function<void(TElement& element)>&& fnIter)
		{
			auto fnFlush = [&fnIter](queue_t* queue)
			{
				while (queue->empty() == false)
				{
					fnIter(queue->front());
					queue->pop();
				}
			};

			// readQueue => writeQueue ������ ���� ó���ϸ鼭 ����.
			fnFlush(readQueue_);
			fnFlush(writeQueue_);
		}

		queue_t*		getReadQueue() const { return readQueue_; }

	private:
		lock_t			lock_;
		lstQueue_t		lstQueue_;
		queue_t*		readQueue_ = nullptr;
		queue_t*		writeQueue_ = nullptr;
	};
}