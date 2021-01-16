#!/bin/bash

set -euo pipefail

bundle exec rake compile
exec ruby "$(dirname "$0")/memory.rb" | ttyplot