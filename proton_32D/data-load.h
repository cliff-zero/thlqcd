#pragma once

#include "data-load-base.h"

namespace qlat
{ //

  inline void clear_all_prop_cache()
  {
    TIMER_VERBOSE("clear_all_prop_cache");
    get_prop_psrc_cache().clear();
  }

  inline void clear_all_data_cache()
  {
    TIMER_VERBOSE("clear_all_data_cache");
    get_point_selection_cache().clear();
    get_field_selection_cache().clear();
    get_point_src_info_cache().clear();
    get_gauge_transform_cache().clear();
    get_psel_prop_cache().clear();
    clear_all_prop_cache();
    // ADJUST ME
    // get_point_distribution_cache().clear();
    // get_does_file_exist_cache().clear();
    //
  }

} // namespace qlat
