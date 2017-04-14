#!/bin/bash

#
# Call autoreconf to recreate the build files. When necessary, invoke
# as `prep_build.bash --force` to force a rebuild of everything.
#

exec autoreconf --install "$@"
