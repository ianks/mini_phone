# frozen_string_literal: true

# rubocop:disable Style/GlobalVars

require 'mkmf'

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

dir_config('mini_phone')

$CXXFLAGS += ' -std=c++11 -ofast '

create_makefile('mini_phone/mini_phone')

# rubocop:enable Style/GlobalVars
