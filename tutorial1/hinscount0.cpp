#include "pin++/Callback.h"
#include "pin++/Instruction_Instrument.h"
#include "pin++/Pintool.h"

#include <fstream>

class docount : public OASIS::Pin::Callback <docount (void)>
{
public:
  docount (void)
    : count_ (0) { }

  void handle_analyze (void)
  {
    ++ this->count_;
  }

  UINT64 count (void) const
  {
    return this->count_;
  }

private:
  UINT64 count_;
};

class Instruction : public OASIS::Pin::Instruction_Instrument <Instruction>
{
public:
  void handle_instrument (const OASIS::Pin::Ins & ins)
  {
    this->callback_.insert (IPOINT_BEFORE, ins);
  }

  UINT64 count (void) const
  {
    return this->callback_.count ();
  }

private:
  docount callback_;
};

class hinscount : public OASIS::Pin::Tool <hinscount>
{
public:
  hinscount (void)
  {
    this->enable_fini_callback ();
  }

  void handle_fini (INT32 code)
  {
    std::ofstream fout ("hinscount.out");
    fout.setf (ios::showbase);
    fout <<  "Count " << this->instruction_.count () << std::endl;

    fout.close ();
  }

private:
  Instruction instruction_;
};

DECLARE_PINTOOL (hinscount);