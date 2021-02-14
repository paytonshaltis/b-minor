This directory contains a number of example tests in the `instructor`
directory to exercise your compiler.  (You should write some additional
tests in the `student` directory as well.) `good*.bminor` should work
correctly, and `bad*.bminor` should not.
Note that the good test files will not necessarily be valid for all phases,
i.e. scanner tests need not parse, typecheck, or compile.
They are just sequences of valid tokens.

You can use scripts like `run_all_tests.sh` (included in `instructor/scanner`)
to quickly test your compiler on
all the tests in a directory, assuming `bminor` is in your `PATH` and produces
a proper exit code from `main()` (which it should!)
