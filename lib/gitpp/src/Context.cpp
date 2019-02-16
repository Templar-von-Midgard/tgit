#include "Context.hpp"

#include <git2/global.h>

namespace gitpp {

Context::Context() noexcept {
  git_libgit2_init();
}

Context::~Context() {
  git_libgit2_shutdown();
}

} // namespace gitpp
