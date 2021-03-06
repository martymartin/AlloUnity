#pragma once

#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

template<typename Data>
class concurrent_queue
{
private:
	std::queue<Data> the_queue;
	mutable boost::mutex the_mutex;
	boost::condition_variable the_condition_variable;
	bool closed;

public:
	concurrent_queue() : closed(false)
	{

	}

	void push(Data const& data)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		the_queue.push(data);
		lock.unlock();
		the_condition_variable.notify_one();
	}

	bool empty() const
	{
		boost::mutex::scoped_lock lock(the_mutex);
		return the_queue.empty();
	}

	bool try_pop(Data& popped_value)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		if (closed || the_queue.empty())
		{
			return false;
		}

		popped_value = the_queue.front();
		the_queue.pop();
		return true;
	}

	bool wait_and_pop(Data& popped_value)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		while (!closed && the_queue.empty())
		{
			the_condition_variable.wait(lock);
		}

		if (closed)
		{
			return false;
		}

		popped_value = the_queue.front();
		the_queue.pop();
		return true;
	}

	void close()
	{
		boost::mutex::scoped_lock lock(the_mutex);
        std::queue<Data> emptyQueue;
        the_queue.swap(emptyQueue); // clears queue
        closed = true;
		the_condition_variable.notify_all();
	}

	size_t size()
	{
		boost::mutex::scoped_lock lock(the_mutex);
		return the_queue.size();
	}

};
