#pragma once

#include <deque>
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>
#include <condition_variable>

class ConsumerProducerCondvar
{
public:

	void perform()
	{
		mProducer = std::thread{ &ConsumerProducerCondvar::produce, this };
		mConsumer = std::thread{ &ConsumerProducerCondvar::consume, this };

		mProducer.join();
		mConsumer.join();
	}

private:

	void produce()
	{
		for (auto i = 10; i >= 0; i--)
		{
			{
				std::unique_lock<std::mutex> locker(mMutex);
				mValues.push_front(i);
			}

			mCond.notify_one();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	void consume()
	{
		int data = -1;

		while (data != 0)
		{

			{
				std::unique_lock<std::mutex> locker(mMutex);

				mCond.wait(locker, [this]() { return (!mValues.empty()); });
				data = mValues.back();
				mValues.pop_back();
			}

			std::cout << "received : " << data << std::endl;
		}
	}

private:

	std::mutex mMutex;

	std::condition_variable mCond;

	std::deque<int> mValues;

	std::thread mProducer;

	std::thread mConsumer;

};
