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

abort 'Missing libphonenumber library' unless have_library('phonenumber')

$CXXFLAGS += ' -std=c++11 '

create_makefile('mini_phone/mini_phone')

# rubocop:enable Style/GlobalVars
