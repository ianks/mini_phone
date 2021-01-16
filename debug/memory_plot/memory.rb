# frozen_string_literal: true

require 'bundler/setup'
require 'mini_phone'
require 'get_process_mem'

10.times do
  GC.start
  GC.compact
end

$stdout.sync = true

loop do
  10_000.times do
    pn = MiniPhone::PhoneNumber.new('+1 404 388 1299')
    pn.e164
    pn.valid?
    pn.possible?
    pn.raw_national
    pn.international
    pn.country_code
    pn.area_code
  end

  4.times { GC.start }

  memory_mb = GetProcessMem.new.mb

  $stdout.puts memory_mb

  break if ARGV.first == '--once'
end
