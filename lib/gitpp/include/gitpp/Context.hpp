#ifndef GITPP_CONTEXT_HPP
#define GITPP_CONTEXT_HPP

namespace gitpp {

struct Context {
  Context() noexcept;
  ~Context();

  Context(const Context&) = delete;
  Context(Context&&) = delete;
  Context& operator=(const Context&) = delete;
  Context& operator=(Context&&) = delete;
};

} // namespace gitpp

#endif
