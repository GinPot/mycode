__all__ = ["main", "tree", "actions"]

import sys
if sys.hexversion < 0x2020000:
  sys.stderr.write('[SKIPPED] at least Python 2.2 is required')

  # note: exiting is a bit harsh for a library module, but we really do
  # require Python 2.2. this package isn't going to work otherwise. and if
  # a user truly wants to use this package under 1.x somehow (or to clean
  # up in some way), then they can always trap the SystemExit exception

  # we're skipping this test, not failing, so exit with 0
  sys.exit(0)

# don't export this name
del sys

# Define True and False if not provided by Python (<=2.1)
try:
  False
except:
  False = 0
  True = 1

class Failure(Exception):
  'Base class for exceptions that indicate test failure'
  pass

class Skip(Exception):
  'Base class for exceptions that indicate test was skipped'
  pass

import main, tree, verify, actions, wc
