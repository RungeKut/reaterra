#ifndef THREADCLASS_H_
#define THREADCLASS_H_

#include <atomic>
#include <thread>


/// Базовый класс управления потоком.
class ThreadClass {

public:

	/// Конструктор.
	ThreadClass();

	/// Деструктор.
	virtual ~ThreadClass();


protected:

	virtual bool process() = 0;
	virtual bool init() {return true;};
	void start();
	void stop();


private:

	std::atomic_bool m_running;
	std::thread m_thread;

	void worker();

};

#endif /* THREADCLASS_H_ */
