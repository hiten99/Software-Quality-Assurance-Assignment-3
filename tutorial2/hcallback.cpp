#include "pin++/Callback.h"
#include "pin++/Pintool.h"
#include "pin++/Buffer.h"
#include "pin++/Trace_Instrument.h"

#include <fstream>
#include <list>

class docount : public OASIS::Pin::Callback <docount (void)>
{
public:
  docount (void)
    : count_ (0),
      ins_count_ (0) { }

  void handle_analyze (void) { ++ this->count_; }
  void ins_count (UINT64 count) { this->ins_count_ = count; }
  UINT64 count (void) const { return this->ins_count_ * this->count_; }

private:
  UINT64 count_;
  UINT64 ins_count_;
};

class Trace : public OASIS::Pin::Trace_Instrument <Trace>
{
public:
  void handle_instrument (const OASIS::Pin::Trace & trace)
  {
    // Allocate a callback for each BBL.
    item_type item (trace.num_bbl ());
    item_type::iterator callback = item.begin ();

    for (const OASIS::Pin::Bbl & bbl : trace)
    {
      callback->ins_count (bbl.ins_count ());
      callback->insert (IPOINT_BEFORE, bbl);

      ++ callback;
    }

    this->traces_.push_back (item);
  }

  UINT64 count (void) const
  {
    UINT64 count = 0;

    for (auto trace : this->traces_)
      for (auto item : trace)
        count += item.count ();

    return count;
  }

private:
  typedef OASIS::Pin::Buffer <docount> item_type;
  typedef std::list <item_type> list_type;

  list_type traces_;
};

class hcallback : public OASIS::Pin::Tool <hcallback>
{
public:
  hcallback (void) { this->enable_fini_callback (); }

  void handle_fini (INT32)
  {
    std::ofstream fout (outfile_.Value ().c_str ());
    fout <<  "Count " << this->trace_.count () << std::endl;
    fout.close ();
  }

private:
  Trace trace_;

  /// @{ KNOBS
  static KNOB <string> outfile_;
  /// @}
};

KNOB <string> hcallback::outfile_ (KNOB_MODE_WRITEONCE, "pintool", "o", "hcallback.out", "specify output file name");

DECLARE_PINTOOL (hcallback);