/**
 * @file bindings/markdown/md_option.hpp
 * @author Ryan Curtin
 *
 * The Markdown option type.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_BINDINGS_MARKDOWN_MD_OPTION_HPP
#define MLPACK_BINDINGS_MARKDOWN_MD_OPTION_HPP

#include <mlpack/core/util/param_data.hpp>
#include <mlpack/core/util/io.hpp>
#include "default_param.hpp"
#include "get_param.hpp"
#include "get_printable_param.hpp"
#include "get_printable_param_name.hpp" // For cli bindings.
#include "get_printable_param_value.hpp" // For cli bindings.
#include "get_printable_type.hpp"
#include "is_serializable.hpp"

namespace mlpack {
namespace bindings {
namespace markdown {

/**
 * The Markdown option class.
 */
template<typename T>
class MDOption
{
 public:
  /**
   * Construct an MDOption object.  When constructed, it will register itself
   * with CLI. The testName parameter is not used and added for compatibility
   * reasons.
   */
  MDOption(const T defaultValue,
           const std::string& identifier,
           const std::string& description,
           const std::string& alias,
           const std::string& cppName,
           const bool required = false,
           const bool input = true,
           const bool noTranspose = false,
           const std::string& bindingName = "")
  {
    // Create the ParamData object to give to CLI.
    util::ParamData data;

    data.desc = description;
    data.name = identifier;
    data.tname = TYPENAME(T);
    data.alias = alias[0];
    data.wasPassed = false;
    data.noTranspose = noTranspose;
    data.required = required;
    data.input = input;
    data.loaded = false;
    // Several options from Python and CLI bindings are persistent.
    if (identifier == "verbose" || identifier == "copy_all_inputs" ||
        identifier == "help" || identifier == "info" || identifier == "version")
      data.persistent = true;
    else
      data.persistent = false;
    data.cppType = cppName;

    // Every parameter we'll get from Markdown will have the correct type.
    data.value = boost::any(defaultValue);

    // Restore the parameters for this program.
    if (identifier != "verbose" && identifier != "copy_all_inputs")
      IO::RestoreSettings(bindingName, false);

    // Set the function pointers that we'll need.  Most of these simply delegate
    // to the current binding type's implementation.  Any new language will need
    // to have all of these implemented, and the Markdown implementation will
    // need to properly delegate.
    IO::GetSingleton().functionMap[data.tname]["DefaultParam"] =
        &DefaultParam<T>;
    IO::GetSingleton().functionMap[data.tname]["GetParam"] = &GetParam<T>;
    IO::GetSingleton().functionMap[data.tname]["GetPrintableParam"] =
        &GetPrintableParam<T>;
    IO::GetSingleton().functionMap[data.tname]["GetPrintableParamName"] =
        &GetPrintableParamName<T>;
    IO::GetSingleton().functionMap[data.tname]["GetPrintableParamValue"] =
        &GetPrintableParamValue<T>;
    IO::GetSingleton().functionMap[data.tname]["GetPrintableType"] =
        &GetPrintableType<T>;
    IO::GetSingleton().functionMap[data.tname]["IsSerializable"] =
        &IsSerializable<T>;

    // Add the option.
    IO::Add(std::move(data));
    if (identifier != "verbose" && identifier != "copy_all_inputs" &&
        identifier != "help" && identifier != "info" && identifier != "version")
      IO::StoreSettings(bindingName);
    IO::ClearSettings();
  }
};

} // namespace markdown
} // namespace bindings
} // namespace mlpack

#endif
