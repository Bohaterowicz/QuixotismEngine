#pragma once

namespace quixotism {
class Profiler {
 public:
  static Profiler& GetProfiler() {
    static Profiler profiler{};
    return profiler;
  }

 private:
  Profiler() {}
};
}  // namespace quixotism
