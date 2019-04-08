#pragma once

#include <boost/property_tree/ptree.hpp>

namespace st
{
  class Input
  {
    public:

      unsigned GetId();

    protected:

      Input(const boost::property_tree::ptree& Tree);

      const unsigned mId;

      const std::string mLabel;

      bool mIsActive;
  };
}
