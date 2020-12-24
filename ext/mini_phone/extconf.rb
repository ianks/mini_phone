# frozen_string_literal: true

# rubocop:disable Style/GlobalVars

require 'mkmf'

abort 'Missing libphonenumber library' unless have_library('phonenumber')

$CXXFLAGS += ' -std=c++11 '

create_makefile('mini_phone/mini_phone')

# rubocop:enable Style/GlobalVars
