#ifndef SYNC_HH_GUARD
#define SYNC_HH_GUARD

#include <mutex>
std::mutex schedule_mutex;

#define __LOCK_WHILE_SCHEDULING__ std::unique_lock<std::mutex> lock_reservoir(schedule_mutex);

#endif
