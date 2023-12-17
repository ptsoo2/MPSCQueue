# MPSCQueue

Directory
- header only
- no lock in read process without spin.
- include: source file
- example: simple thread process

### TODO
- 

## EXAMPLE
```c
#include <conio.h>
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
```
## OUTPUT
```c
push: 10
push: 0
push: 1
push: 2
push: 3
push: 4
push: 5
push: 6
push: 7
push: 8
push: 9
push: 20
push: 21
push: 22
push: 23
push: 24
push: 25
push: 26
push: 27
push: 28
push: 29
pop: 10
pop: 0
pop: 1
pop: 2
pop: 3
pop: 4
pop: 5
pop: 6
pop: 7
pop: 8
pop: 9
pop: 20
pop: 21
pop: 22
pop: 23
pop: 24
pop: 25
pop: 26
pop: 27
pop: 28
pop: 29
push: 11
push: 12
push: 13
push: 14
push: 15
push: 16
push: 17
push: 18
push: 19
pop: 11
pop: 12
pop: 13
pop: 14
pop: 15
pop: 16
pop: 17
pop: 18
pop: 19
