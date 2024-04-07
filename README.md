# glycerine

On webOS 1, there doesn't seem to be any way to enable
["dev logs"](https://www.webosbrew.org/pages/logging.html)—i.e., disable the
whitelist in `PmLogDaemon`. Instead, let's just add everything to the
whitelist... or at least make `PmLogDaemon` *believe* everything is
whitelisted.

## Theory

My first attempt was to just always return something non-null from
`g_hash_table_lookup()`, since the only time it's used in `PmLogDaemon` is for
checking the whitelist. However, it's unfortunately also used by some
dependencies (in particular, `libpbnjson`), so it needs to actually work.

I thought about checking which module `g_hash_table_lookup()` was being called
from, but I'm too lazy for that. Instead, I decided to just grab the first hash
table created with `g_hash_table_new()` that uses the default string hash table
functions `g_str_hash` and `g_str_equal`. The creation of the whitelist hash
table occurs early in the lifetime of `PmLogDaemon`—it may well be the very
first one, but the extra checks don't hurt. Anyway, when
`g_hash_table_lookup()` is called on the previously recorded hash table, I just
return a non-null value. It's not a valid pointer, but `PmLogDaemon` only cares
whether it's null and does not dereference it. If the wrong table is somehow
chosen, the program will almost certainly crash.

## Building

Use [buildroot-nc4](https://github.com/openlgtv/buildroot-nc4).

For example, if it's located in `/opt/arm-webos-linux-gnueabi_sdk-buildroot/`,
run:

```sh
make CROSS_COMPILE=/opt/arm-webos-linux-gnueabi_sdk-buildroot/bin/arm-webos-linux-gnueabi-
```

## Using

If you kill `PmLogDaemon` a bunch of times quickly, Upstart will stop trying to
restart it. Then you can start it yourself with:

```sh
LD_PRELOAD=/path/to/libglycerine.so /usr/sbin/PmLogDaemon -z -f 6 -m
```

The arguments are from `/etc/init/PmLogDaemon.conf`.

Although `PmLogDaemon --help` mentions `-v`/`--verbose`, it doesn't seem to do
anything (at least on stdout/stderr).

## Debugging

To debug using `gdb`:

```sh
gdb --args /usr/sbin/PmLogDaemon -z -f 6 -m
set exec-wrapper env LD_PRELOAD=/path/to/libglycerine.so
run
```

## License

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>.

See `LICENSE` for details.
