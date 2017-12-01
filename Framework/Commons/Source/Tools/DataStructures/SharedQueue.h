/*
   Copyright 2017 Thomas Krause <thomaskrause@posteo.de>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef _SharedQueue_h_
#define _SharedQueue_h_

#include <queue>
#include <mutex>
#include <condition_variable>

namespace naoth
{
  /**
   * This is a thread-safe queue that has a blocking pop() function.
   *
   * It is possible to close a queue. If a queue is closed, no new entries
   * can be added: as soon as the queue is empty the pop() funtion will return immediatly instead of waiting forever.
   * Closing the queue can't be undone.
   */
  template<typename T>
  class SharedQueue
  {
  public:

    SharedQueue()
    : isClosed(false)
    {

    }

    /**
     * @brief Retrieve an item from the queue. This will block until an item is available. If the queue is empty
     * and closed it will return immediatly with "false" as a result.
     * @param item
     * @return "true" if an item was retrieved from the queue, false if not.
     */
    bool pop(T& item)
    {
      std::unique_lock<std::mutex> lock(queueMutex);

      addedCondition.wait(lock, [this] {return this->isClosed || !this->queue.empty();});
      if(isClosed && queue.empty())
      {
        // queue is empty and since it is closed no new entries will be added.
        return false;
      }

      item = std::move(queue.front());
      queue.pop();

      lock.unlock();

      return true;
    }

    /**
     * @brief Try to retrieve an item from the queue. This will *not* block until an item is available.
     * If the queue is empty or closed it will return immediatly with "false" as a result.
     * @param item
     * @return
     */
    bool try_pop(T& item)
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      if(isClosed && queue.empty())
      {
        return false;
      }
      item = std::move(queue.front());
      queue.pop();
      lock.unlock();

      return true;
    }

    void push(T&& item)
    {
      std::unique_lock<std::mutex> lock(queueMutex);

      if(isClosed)
      {
        return;
      }

      queue.emplace(item);
      lock.unlock();
      addedCondition.notify_one();
    }

    void push(const T& item)
    {
      std::unique_lock<std::mutex> lock(queueMutex);

      if(isClosed)
      {
        return;
      }

      queue.push(item);
      lock.unlock();
      addedCondition.notify_one();
    }

    /**
     * Close this queue which means it will not accept any new items.
     */
    void close()
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      if(!isClosed)
      {
        isClosed = true;

        lock.unlock();

        addedCondition.notify_all();
      }
    }

    /**
     * @brief Remove all entries from the queue but do not close it.
     */
    void clear()
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      while(!queue.empty())
      {
        queue.pop();
      }
    }

    bool empty()
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      return queue.empty();
    }

    size_t size()
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      return queue.size();
    }


  private:

    bool isClosed;

    std::queue<T> queue;

    std::mutex queueMutex;
    std::condition_variable addedCondition;

  };
}

#endif // _SharedQueue_h_
