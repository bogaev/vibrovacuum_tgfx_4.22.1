#include "os_tasks_wrapper.h"
#include "stdio.h"

RTOSTaskWrapper::RTOSTaskWrapper(const char* task_name,
                                 uint32_t stack_size,
                                 osPriority_t priority,
                                 std::function<void(void*)> task_code)
: task_code_(std::move(task_code))
{
  attr_.name = task_name;
  attr_.stack_size = stack_size;
  attr_.priority = priority;
}

RTOSTaskWrapper::~RTOSTaskWrapper() {
  Delete();
}

void RTOSTaskWrapper::Create() {
  handle_ = osThreadNew(TaskWrapper, this, &attr_);
}

void RTOSTaskWrapper::Delete() {
  osThreadTerminate(handle_);
}

void RTOSTaskWrapper::TaskWrapper(void* argument) {
  static_cast<RTOSTaskWrapper*>(argument)->task_code_(nullptr);
}
