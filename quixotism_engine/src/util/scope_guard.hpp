#pragma once

namespace quixotism {

template <typename FUNC>
class ScopeGuard {
 public:
  ScopeGuard(FUNC &&func) : func{std::forward<FUNC>(func)}, engaged{true} {}
  ~ScopeGuard() {
    if (engaged) {
      func();
    }
  }

  void Disengage() { engaged = false; }
  void Engage() { engaged = true; }

 private:
  bool engaged;
  FUNC func;
};

}  // namespace quixotism
