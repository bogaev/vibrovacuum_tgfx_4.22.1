#ifndef _OBSERVER_H_
#define _OBSERVER_H_

#include <list>
#include <algorithm>
#include <functional>

namespace _pattern {

template<typename T>
class Observer {
 public:
   virtual void Update(T data) = 0;
};

template<typename T>
class Publisher {
 public:
  Publisher() {
  }
   
  void RegisterObserver(Observer<T>* observer) {
    observers_.push_back(observer);
  }

  void RemoveObserver(Observer<T>* observer) {
    observers_.remove(observer);
  }

  void Notify(T data) {
    for (Observer<T>* observer : observers_) {
      observer->Update(std::move(data));
    }
  }

 private:
  std::list<Observer<T>*> observers_;
};

} // namespace __utility

#endif // _OBSERVER_H_