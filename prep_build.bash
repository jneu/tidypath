#!/bin/bash

#
# Call autoreconf to recreate the build files. When necessary, invoke
# as `prep_build.bash --force` to force a rebuild of everything.
#

set -o errexit

if ! [[ -f README.md ]]; then
    echo "prep_build.bash must be run from the top level directory" 1>&2
    exit 1
fi

# reduce warnings by pre-creating the m4 directory
mkdir -p m4

exec autoreconf --install "$@"
