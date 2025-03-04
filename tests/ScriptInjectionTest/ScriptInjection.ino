#include <epoxy_test/Script/Script.h>
#include <epoxy_test/Script/Commands/BitStream.h>
#include <Arduino.h>
#include <AUnit.h>
#include <aunit/Test.h>

using aunit::TestRunner;

test(ScriptInjection, multiple_direct_scripts)
{
  EpoxyTest::reset();
  EpoxyTest::Script s1("script:pin D1 0");
  EpoxyTest::Script s2("script:at 100ms pin D1 1");
  EpoxyTest::Script s3("script:at 200ms pin D1 0");

  assertEqual(digitalRead(1), 0);
  while(millis() <= 110) delay(10);
  assertEqual(digitalRead(1), 1);
  while(millis() <= 210) delay(10);
  assertEqual(digitalRead(1), 0);
}

//---------------------------------------------------------------------------
test(ScriptInjection, pin)
{
  EpoxyTest::reset();
  EpoxyTest::Script script("file:scripts/script.txt");

  assertEqual(digitalRead(1), 0);
  while(millis() <= 110) delay(10);
  assertEqual(digitalRead(1), 1);
}

test(ScriptInjection, pin_mapping)
{
  EpoxyTest::reset();
  EpoxyTest::Script script("scripts/mapping.txt");

  assertEqual(digitalRead(1), 0);
  while(millis() <= 110) delay(10);
  assertEqual(digitalRead(SS), 1);
  assertEqual(digitalRead(1), 1);
}

test(ScriptInjection, at)
{
  EpoxyTest::reset();
  EpoxyTest::Script script("scripts/at.txt");

  assertEqual(digitalRead(4), 0);
  while(millis() <= 550) delay(10);
  assertEqual(digitalRead(4), 1);
}

test(ScriptInjection, bitstream)
{
  EpoxyTest::reset();
  EpoxyTest::Script bitstream("scripts/bitstream.txt");

  struct streamRead
  {
    int pin;              // pin of bitstream
    int period;           // period
    unsigned long peek;   // next time to sample bit
    size_t size;          // Number of bits to read
    std::string stream;   // rebuilt of bitstream
    bool invalid_;         // detect out of sync

    unsigned long end_sampling()
    {
      return (size+1) * period;
    }

    /** Check that the jitter of the injector allowed the test to
      * run ok
      */
    bool invalid()
    {
      return invalid_ or EpoxyInjection::Injector::maxJitter() > period / 3;
    }

    streamRead(int pin, int period, size_t size)
      : pin(pin), period(period)
      , peek(0)
      , size(size)
      , invalid_(false)
    {}

    // Note: this is a kind of software serial emulation that could
    // be re-used
    void sample()
    {
      int read_pin = digitalRead(pin);
      auto us = micros();
      if (stream.length() == size) return;
      if (peek == 0)
      {
        if (read_pin == 0) return;  // wait start bit
        peek = us + period + period/2;
      }
      else if (peek > us)
      {
        return;
      }
      else
      {
        int delta = us - peek;
        int max_allowed = period / 3;
        if (delta > max_allowed)
        {
          std::cerr << "Invalid sample : d=" << delta << ", max=" << max_allowed << ", us=" << us << ", expected: " << peek << std::endl;
          invalid_ = true;
        }
        peek += period;
      }

      if (read_pin == 1)
        stream +='1';
      else
        stream +='0';

    }

  };

  // see bitstream.txt for values
  streamRead stream_1(5, 10000, 16);
  streamRead stream_2(6, 15000, 16);

  auto end_sampling = std::max(stream_1.end_sampling(), stream_2.end_sampling());

  while(micros() < end_sampling)
  {
    stream_1.sample();
    stream_2.sample();
  }

  // Disable test if any sync problem has occured (machine to slow ?)
  if (EpoxyTest::BitStream::outOfSyncCount() or stream_1.invalid() or stream_2.invalid())
  {
    std::cerr << "*** Warning: test cancelled due to outOfSync errors" << std::endl;
    std::cerr << "Max jitter : " << EpoxyInjection::Injector::maxJitter() << std::endl;
    std::cerr << "Periods : " << stream_1.period << ", " << stream_2.period << std::endl;
    return;
  }

  assertEqual(stream_1.stream.c_str(), "1001110111010110");
  assertEqual(stream_2.stream.c_str(), "1101001010010101");
}

//---------------------------------------------------------------------------

void setup() {
  aunit::Test::displayMinPosition(50);
#if ! defined(EPOXY_DUINO)
  delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // needed for Leonardo/Micro
}

void loop() {
  TestRunner::run();
}
