#pragma once
#include <intrin.h>

#include <string>
#include <unordered_map>

#include "quixotism_c.hpp"

namespace quixotism {

class Profiler {
 public:
  static Profiler& GetProfiler() {
    static Profiler profiler{};
    return profiler;
  }

  void RegisterTiming(std::string& name, u64 cycles) {
    registered_timings[name] = cycles;
  }

 private:
  Profiler() {}

  std::unordered_map<std::string, u64> registered_timings;
};

class ScopeTimer {
 public:
  ScopeTimer(std::string _name) : name{_name} { start_cycle = __rdtsc(); }
  ~ScopeTimer() {
    auto cycles_elapsed = __rdtsc() - start_cycle;
    Profiler::GetProfiler().RegisterTiming(name, cycles_elapsed);
  }

 private:
  std::string name;
  u64 start_cycle;
};
}  // namespace quixotism
