// quick-lint-js finds bugs in JavaScript programs.
// Copyright (C) 2020  Matthew Glazar
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cerrno>
#include <clocale>
#include <cstring>
#include <libintl.h>
#include <quick-lint-js/assert.h>
#include <quick-lint-js/have.h>
#include <quick-lint-js/translation.h>

#if QLJS_HAVE_SETENV
#include <stdlib.h>
#endif

#if QLJS_HAVE_SETENVIRONMENTVARIABLE
#include <Windows.h>
#endif

namespace quick_lint_js {
namespace {
constexpr const char gettext_domain[] = "quick-lint-js";

bool initialized_translations = false;

void initialize_translations_from_environment_unsafe(const char* locale_dir) {
  if (!std::setlocale(LC_ALL, "")) {
    std::fprintf(stderr, "warning: failed to set locale: %s\n",
                 std::strerror(errno));
  }
  if (!::bindtextdomain(gettext_domain, locale_dir)) {
    std::fprintf(stderr, "warning: failed to set translations directory: %s\n",
                 std::strerror(errno));
  }
  initialized_translations = true;
}
}

const char8* translate(const char* message) {
  // HACK(strager): Assume gettext's output encoding is UTF-8.
  return reinterpret_cast<const char8*>(::dgettext(gettext_domain, message));
}

void initialize_translations_from_environment(const char* locale_dir) {
  QLJS_ASSERT(!initialized_translations);
  initialize_translations_from_environment_unsafe(locale_dir);
}

void initialize_translations_from_locale(const char* locale_dir,
                                         const char* locale_name) {
  QLJS_ASSERT(locale_name[0] != '\0');

#if QLJS_HAVE_SETENV
  if (::setenv("LANGUAGE", locale_name, 1) == -1) {
    std::fprintf(stderr, "warning: failed to set language: %s\n",
                 std::strerror(errno));
  }
#elif QLJS_HAVE_SETENVIRONMENTVARIABLE
  // @@@ test.
  if (!::SetEnvironmentVariableA("LANGUAGE", locale_name)) {
    std::fprintf(stderr, "warning: failed to set language\n");
  }
#else
#error "Unknown platform"
#endif
  initialize_translations_from_environment_unsafe(locale_dir);
}
}
