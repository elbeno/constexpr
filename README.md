# Skeleton

A quick project skeleton for easily starting new C++ projects and experiments. A test and library project is provided, using `scons` to build.

## To make a new project

```bash
~/skeleton $ . new.sh foo
```

This copies the contents of `skeleton` to a sibling directory `foo`, creates an empty git repo there, and drops you there to start hacking.

If you want to have a header-only deal, you can run `header-only.sh` to get rid of the default library project.

`clean.sh` is provided to completely clean up the results of running `scons`.

Happy hacking!
