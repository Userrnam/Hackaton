#include "Job_System.hpp"

#include <iostream>

void Job_System::worker() {
	while (!_stop) {
		execute_job();
	}
}

void Job_System::create(int num_threads) {
	for (int i = 0; i < num_threads; ++i) {
		_threads.push_back(new std::thread(&Job_System::worker, this));
	}
}

void Job_System::destroy() {
	_stop = true;
	for (auto thread : _threads) {
		thread->join();
		delete thread;
	}
}

bool Job_System::execute_job() {
	_job_available_mutex.lock();
	if (_jobs.size()) {
		Job job = _jobs.front();
		_jobs.pop();

		_job_available_mutex.unlock();

		_working_threads++;
		job.execute(job.user_data);
		_working_threads--;
		return true;
	}
	_job_available_mutex.unlock();
	return false;
}

bool Job_System::done() {
	return _jobs.empty();
}

void Job_System::wait_for_complition() {
	while (_jobs.size() || _working_threads.load()) {
		// this is dumb. FIXME
	}
}

void Job_System::clear_jobs() {
	std::lock_guard<std::mutex> guard(_job_available_mutex);

	std::queue<Job> empty;
	std::swap(_jobs, empty);
}

void Job_System::add_job(const Job& job) {
	std::lock_guard<std::mutex> guard(_job_available_mutex);
	_jobs.push(job);
}
