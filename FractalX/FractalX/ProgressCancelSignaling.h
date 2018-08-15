#pragma once
#include <mutex>

//static std::atomic_bool _canceled = false;

struct CancelEvent
{
//	std::condition_variable CancelCv;
//	std::mutex CancelMutex;
	std::atomic_bool Canceled = false;
};
