/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the
  terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
  details.

  You should have received a copy of the GNU Lesser General Public License along
  with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include "cxxet/mark/metadata.hxx"

#include "impl/event/kind/metadata.hxx"
#include "impl/event/kind/metadata_type.hxx"
#include "impl/thread_local_sink_submit_event.hxx"

namespace cxxet::mark {

void process_name(unsigned const categories,
                  char const *const proc_name) noexcept {
  impl::thread_local_sink_submit_event(impl::event::metadata{
      categories, proc_name, impl::event::metadata_type::process_name});
}

void process_label(unsigned const categories,
                   char const *const proc_label) noexcept {
  impl::thread_local_sink_submit_event(impl::event::metadata{
      categories, proc_label, impl::event::metadata_type::process_labels});
}

void process_sort_index(unsigned const categories,
                        int const proc_sort_index) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::metadata{categories, proc_sort_index,
                            impl::event::metadata_type::process_sort_index});
}

void thread_name(unsigned const categories,
                 char const *const th_name) noexcept {
  impl::thread_local_sink_submit_event(impl::event::metadata{
      categories, th_name, impl::event::metadata_type::thread_name});
}

void thread_sort_index(unsigned const categories,
                       int const th_sort_index) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::metadata{categories, th_sort_index,
                            impl::event::metadata_type::thread_sort_index});
}

} // namespace cxxet::mark
