#/bin/sh

set -e

bundle exec rake compile

exec valgrind \
    --tool=memcheck \
    --num-callers=15 \
    --partial-loads-ok=yes \
    --undef-value-errors=no \
    --leak-check=full \
    --log-file=valgrind-out.txt \
    ruby -Ilib "$@"
