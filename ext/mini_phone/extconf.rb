# frozen_string_literal: true

# rubocop:disable Style/GlobalVars

require 'mkmf'

LIBDIR      = RbConfig::CONFIG['libdir']

INCLUDEDIR  = RbConfig::CONFIG['includedir']

header_dirs = [
  # First search /opt/local for macports
  '/opt/local/include',

  # Then search /usr/local for people that installed from source
  '/usr/local/include',

  # Check the ruby install locations
  INCLUDEDIR,

  # Finally fall back to /usr
  '/usr/include'
]

lib_dirs = [
  # First search /opt/local for macports
  '/opt/local/lib',

  # Then search /usr/local for people that installed from source
  '/usr/local/lib',

  # Check the ruby install locations
  LIBDIR,

  # Finally fall back to /usr
  '/usr/lib'
]

# Detect homebrew installs
if find_executable('brew')
  brew_prefix = `brew --prefix`.strip
  header_dirs.unshift "#{brew_prefix}/include"
  lib_dirs.unshift "#{brew_prefix}/lib"
end

dir_config('mini_phone', header_dirs, lib_dirs)

unless have_library('phonenumber')
  abort <<~MSG

    ,----------------------------------------------------------------------,
    |                                                                      |
    | Hey there, sorry for the inconvenience!                              |
    |                                                                      |
    | It looks like libphonenumber is not installed on this system. Don't  |
    | worry, this should be easy to fix:                                   |
    |                                                                      |
    | 1. Install the lib                                                   |
    |                                                                      |
    |  On Mac:                                                             |
    |    brew install libphonenumber                                       |
    |                                                                      |
    |  On Debian / Ubuntu:                                                 |
    |    apt-get install -y libphonenumber                                 |
    |                                                                      |
    | 2. Retry installing the gem (i.e `bundle install`)                   |
    |                                                                      |
    '----------------------------------------------------------------------'

  MSG
end

$CXXFLAGS += ' -std=c++11 -ofast '

create_makefile('mini_phone/mini_phone')

# rubocop:enable Style/GlobalVars
