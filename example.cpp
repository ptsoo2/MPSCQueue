#include "MPSCQueue.h"

#include <thread>

TSUtil::CMPSCQueue<size_t> queue;

int main()
{
	// example
	const size_t threadCount = 3;
	const size_t pushCount = 10;

	const size_t maxPopCount = threadCount * pushCount;

	// consumer
	std::jthread consumeThread
	{
		[]()
		{
			size_t count = 0;
			while (count < maxPopCount)
			{
				if (queue.swap() == false)
					continue;

				auto readQueue = queue.getReadQueue();
				while (readQueue->empty() == false)
				{
					printf("pop: %zu \n", readQueue->front());
					readQueue->pop();
					++count;
				}
			}
		}
	};

	// producer
	std::vector<std::jthread> lstPublishThread;
	lstPublishThread.reserve(threadCount);

	for (size_t i = 0; i < threadCount; ++i)
	{
		lstPublishThread.emplace_back(
			[tid = i]()
			{
				const size_t offset = tid * pushCount;
				for (size_t i = 0; i < pushCount; ++i)
				{
					printf("push: %zu \n", offset + i);
					queue.push(offset + i);
				}
			}
		);
	}

	return 0;
}
