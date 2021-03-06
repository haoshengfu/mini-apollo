/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "timer_task.h"

#include "../task/task.h"

namespace apollo {
namespace cyber {

void TimerTask::Fire(bool async) {
  if (status_ != INIT) {
    return;
  }
  if (oneshot_)  // not repeat. so always on ready
    status_ = EXPIRED;
  if (async) {
    cyber::Async(handler_);
  } else {
    handler_();
  }
}

bool TimerTask::Cancel() {
  if (State() != INIT) {
    return false;
  }
  status_ = CANCELED;
  return true;
}
}  // namespace cyber
}  // namespace apollo
