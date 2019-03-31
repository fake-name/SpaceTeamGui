#pragma once

#include "Input.hpp"

namespace st
{
  struct Success;

  class Digital : public st::Input
  {
    public:

      Digital(const boost::property_tree::ptree& Tree);

      std::string GetNewCommand();

      void IsCorrect(st::Success&);

      void SetCurrentState(bool State);

      void Update();

    private:

      bool IsInCorrectState() const;

      bool mDesiredState;

      bool mCurrentState;

      const std::string mOnLabel;

      const std::string mOffLabel;
  };
}
