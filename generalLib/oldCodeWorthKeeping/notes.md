# trimSamHeaderOnly

This used the samEntry struct before I did the .c upgrade.
  It was a bit faster (maybe 10%). This is likely due to
  changes in samEntyStruct.h. Maybe do to using void, or
  because I had to make the print statements non-macros.
