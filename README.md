# tidypath

`tidypath` cleans up PATH and PATH-like strings by removing duplicates and
normalizing empty fragments. Add the following, or its equivalent, to the
bottom of the appropriate startup script (e.g., `~/.profile`,
`~/.bash_profile`) for your shell:
```
hash tidypath && export PATH="`tidypath`"
```

## Installation
If you use the [Homebrew](https://brew.sh/) package manager on macOS, you
can add the `tidypath` tap and install directly:
```
$ brew tap jneu/tidypath
$ brew install tidypath
```

`tidypath` uses the GNU build system. To build directly from the github source,
you'll need Autoconf, Automake, and Libtool. Execute the following:
```
$ git clone https://github.com/jneu/tidypath.git
$ cd tidypath
$ ./bootstrap
$ ./configure
$ make
$ make check
$ sudo make install
```
If you received `tidypath` as a tarball, you don't need the GNU build tools.
Instead, execute the following:
```
$ tar xzf tidypath-0.1.1.tar.gz    # Your version may be different
$ cd tidypath-0.1.1
$ ./configure
$ make
$ make check
$ sudo make install
```

Standard `configure` options are supported. E.g, to install `tidypath` in a
specific directory, use something like:
```
$ configure --prefix=/home/your_user_name/local
``` 

By default, `tidypath` does not free any internally allocated memory as it's
not necessary to do so and slows down program execution. When developing or
troubleshooting, it's useful to change this behavior. To insure all memory is
deallocated, use:
```
$ configure --enable-free
```

## Usage
PATH variables are notorious for becoming large and unwieldy. `tidypath` cleans
up PATH and PATH-like strings by removing duplicate entries and changing all
empty fragments into `.`. Typical usage would be to add something like the
following to the appropriate shell startup script:
```
hash tidypath && export PATH="`tidypath`"
```

Other examples:
```
$ export PATH=/usr/bin:/usr/local/bin:/bin:/usr/local/bin:
$ tidypath
/usr/bin:/usr/local/bin:/bin:. # No newline
$ tidypath "$PATH"             # Equivalent to the above
$ tidypath --env PATH          # Equivalent to the above
$ tidypath --env CLASSPATH     # Operate on $CLASSPATH instead
$ tidypath --aggressive        # _Really_ clean up $PATH
```

`tidypath` supports command line options:
```
tidypath [options] [pathlike_string (defaults to $PATH)]
    -a, --aggressive          also prune absolute fragments that match previous
                              inodes, do not exist, or are not directories
    -d, --delimiter <char>    specify the delimiter to use; defaults to :
    -e, --env                 the first argument will specify the name of an
                              environment variable to use
    -h, --help                show this help message and exit
    -i, --ignore-empty        prune empty fragments
    -n, --nop                 do not prune any fragments for any reason
    -p, --pretty              print each fragment separated by a newline
    -V, --version             show the version of tidypath and exit
```
