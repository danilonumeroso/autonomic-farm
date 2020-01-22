#ifndef SYNC_HH_GUARD
#define SYNC_HH_GUARD

#include <condition_variable>

namespace spm {
  std::condition_variable can_emit;
  std::mutex emitter_mutex;
  std::mutex sleeping_workers_queue_mutex;
  std::mutex reservoir_mutex;
  std::mutex output_stream;
}

#define __WAIT_FOR_WORKERS__ std::unique_lock<std::mutex> lock_emitter(spm::emitter_mutex); \
    spm::can_emit.wait(lock_emitter);
#define __NOTIFY_EMITTER__ spm::can_emit.notify_one();

#define __LOCK_SLEEPING_WORKERS_QUEUE__ std::unique_lock<std::mutex> lock_workers_queue(sleeping_workers_queue_mutex);

#define __LOCK_RESERVOIR__ std::unique_lock<std::mutex> lock_reservoir(reservoir_mutex);

#define __LOCK_OUTPUT_STREAM__ std::unique_lock<std::mutex> lock_output_stream(output_stream);

#endif
