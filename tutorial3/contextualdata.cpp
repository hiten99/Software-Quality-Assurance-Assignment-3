#include "pin++/Instruction_Instrument.h"
#include "pin++/Callback.h"
#include "pin++/Pintool.h"
#include <fstream>

class printip : public OASIS::Pin::Callback < printip (OASIS::Pin::ARG_INST_PTR) >
{
public:
  printip (FILE * file)
    : file_ (file) { }

  void handle_analyze (param_type1 addr)
  {
    ::fprintf (this->file_, "0x%p\n", addr);
  }

private:
  FILE * file_;
};

class Instrument : public OASIS::Pin::Instruction_Instrument <Instrument>
{
public:
  Instrument (FILE * file)
    : printip_ (file) { }

  void handle_instrument (const OASIS::Pin::Ins & ins)
  {
    this->printip_.insert (IPOINT_BEFORE, ins);
  }

private:
  printip printip_;
};

class contextualdata : public OASIS::Pin::Tool <contextualdata>
{
public:
  contextualdata (void)
    : file_ (fopen ("contextualdata.out", "w")),
      inst_ (file_)
  {
    this->enable_fini_callback ();
  }


  void handle_fini (INT32)
  {
    fprintf (this->file_, "#eof\n");
    fclose (this->file_);
  }

private:
  FILE * file_;
  Instrument inst_;
};

DECLARE_PINTOOL (contextualdata);