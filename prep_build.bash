#!/bin/bash

set -o errexit

aclocal
automake --foreign --add-missing --copy
autoconf
