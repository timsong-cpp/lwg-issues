#ifndef INCLUDE_LWG_STATUS_H
#define INCLUDE_LWG_STATUS_H

// standard headers
#include <string_view>
#include <cstddef>

namespace lwg
{
auto filename_for_status(std::string_view stat) -> std::string_view;

auto get_status_priority(std::string_view stat) noexcept -> std::ptrdiff_t;

auto is_active(std::string_view stat) -> bool;
auto is_active_not_ready(std::string_view stat) -> bool;
auto is_defect(std::string_view stat) -> bool;
auto is_closed(std::string_view stat) -> bool;
auto is_tentative(std::string_view stat) -> bool;
auto is_not_resolved(std::string_view stat) -> bool;
auto is_assigned_to_another_group(std::string_view stat) -> bool;
auto is_votable(std::string_view stat) -> bool;
auto is_ready(std::string_view stat) -> bool;

// Functions to "normalize" a status string
auto remove_pending(std::string_view stat) -> std::string_view;
auto remove_tentatively(std::string_view stat) -> std::string_view;
auto remove_qualifier(std::string_view stat) -> std::string_view;


} // close namespace lwg

#endif // INCLUDE_LWG_STATUS_H
