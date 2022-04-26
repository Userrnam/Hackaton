#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <vector>


// I do not use abstract class to avoid memory allocation
typedef void(*Job_Function)(void* user_data);
struct Job {
	void* user_data;
	Job_Function execute;
};

class Job_System {
	std::queue<Job> _jobs;
	std::vector<std::thread*> _threads;
	std::mutex _job_available_mutex;
	std::atomic<int> _working_threads;
	bool _stop = false;

	void worker();
public:
	void create(int num_threads);
	void destroy();

	// execute a job from queue at current thread
	// returns false if no jobs available
	bool execute_job();

	// job system completed all jobs
	bool done();

	void wait_for_complition();
	void clear_jobs();
	void add_job(const Job& job);
};
