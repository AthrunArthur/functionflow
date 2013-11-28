#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

typedef unsigned int uintptr_t;
#include <boost/cstdint.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/container/deque.hpp>
#include <vector>

namespace ff{
template<size_t MIN, class T>
class mutex_stealing_queue : public boost::noncopyable
{
public:
    mutex_stealing_queue() { }

    void push_back(const T & val)
    {
        boost::unique_lock<boost::mutex> ul(m_oMutex);
        m_oContainer.push_back(val);
    }

    bool pop(T & val)
    {
        boost::unique_lock<boost::mutex> ul(m_oMutex);

        if(m_oContainer.empty())
            return false;
        if( m_oContainer.size() > MIN &&
                m_oContainer.size() < m_oContainer.max_size()/4)
            m_oContainer.shrink_to_fit();
        val = m_oContainer.front();
        m_oContainer.pop_front();
        return true;
    }

    bool steal(T &val)
    {
        boost::unique_lock<boost::mutex> ul(m_oMutex);

        if(m_oContainer.empty())
            return false;
        val = m_oContainer.back();
        m_oContainer.pop_back();
        return true;
    }

    size_t size() const
    {
        boost::unique_lock<boost::mutex> ul(m_oMutex);
        return m_oContainer.size();
    }
    bool empty() const
    {
        boost::unique_lock<boost::mutex> ul(m_oMutex);
        return m_oContainer.empty();
    }
protected:
    mutable boost::mutex  m_oMutex;
    boost::container::deque<T> m_oContainer;
};//end class mutex_stealing_queue



template <class T>
class threadpool : public boost::noncopyable
{
public:
    typedef T task_t;
    typedef boost::function<void (task_t &)> runner_t;
    typedef mutex_stealing_queue<256, T> stealing_queue_t;
    typedef boost::shared_ptr<boost::thread> thrd_ptr;
    typedef stealing_queue_t * stealing_queue_ptr;

    template <class task_runner_t>
    threadpool(int thrd_num, const task_runner_t & f)
        : m_thrd_num(thrd_num)
        , m_func(f)
        , m_threads()
        , m_task_queues()
        , to_quit(false)
        , is_running(false)
        , m_gen()
        , m_dist(0, thrd_num)
        , m_die(m_gen, m_dist) {}

    void		run()
    {
        for(int i = 0; i < m_thrd_num; i++)
        {
            m_task_queues.push_back(new stealing_queue_t());
        }

        is_running = true;
        for (int i = 0; i< m_thrd_num ; i++)
        {
            m_threads.push_back(
                make_thread(
                    boost::bind(&threadpool<T>::thrd_run, this, i)));
        }
    }

    void		schedule(const T & task)
    {
        int id = m_die()%m_thrd_num;
        m_task_queues[id]->push_back(task);
    }


    void		join()
    {
        to_quit.store(true);
        for(size_t i = 0; i< m_threads.size(); ++i)
        {
            if(m_threads[i]->joinable())
                m_threads[i]->join();
        }

        for(size_t i = 0; i < m_task_queues.size(); ++i)
        {
            delete m_task_queues[i];
        }
        is_running = false;
    }

    bool		is_active()
    {
        return is_running.load();
    }

protected:
    void		thrd_run(int id)
    {
        stealing_queue_ptr q = m_task_queues[id];
        task_t task;
        while(!to_quit.load() || !is_task_queue_null())
        {
            bool b = q->pop(task);
            if(b)
            {
                m_func(task);
            }
            else {
                b = steal_and_run(id);
            }

            if(!b)
                boost::thread::yield();
        }
    }

    bool		steal_and_run(int id)
    {
        bool b =false;
        task_t task;
        for(int i = (id +1)%m_thrd_num; i!= id; i=(i+1)%m_thrd_num)
        {
            stealing_queue_ptr q = m_task_queues[i];
            b = q->pop(task);
            if(b)
            {
                m_func(task);
                return true;
            }
        }
        return false;
    }

    bool 		is_task_queue_null()
    {
        for(int i = 0; i< m_thrd_num; i++)
        {
            if(!m_task_queues[i]->empty())
                return false;
        }
        return true;
    }

private:
    template <class Func_t>
    thrd_ptr make_thread(const Func_t & f)
    {
        return thrd_ptr(new boost::thread(f));
    }
protected:
    int m_thrd_num;
    boost::function<void (T &)> m_func;


    std::vector<thrd_ptr> m_threads;
    std::vector<stealing_queue_ptr> m_task_queues;
    boost::atomic<bool>		to_quit;
    boost::atomic<bool>		is_running;
    boost::mt19937 m_gen;
    boost::uniform_int<> m_dist;
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >m_die;
};//end class threadpool

}//end namespace ff;


#endif