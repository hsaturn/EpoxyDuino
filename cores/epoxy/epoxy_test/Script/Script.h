#pragma once

#include <epoxy_test/injection>

#include "ScriptParser.h"
#include <string>
#include <iostream>

namespace EpoxyTest
{

class Script : public ScriptParser
{
  public:
    Script(const std::string filename);
    ~Script();

    using EventPtr = std::unique_ptr<EpoxyInjection::Event>;

  private:
    friend class ScriptEvent;
    EventPtr step(unsigned long delay);

  private:
    std::istream* input = nullptr;
};

}
