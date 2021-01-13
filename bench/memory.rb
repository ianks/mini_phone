# frozen_string_literal: true

require 'mini_phone'
require 'get_process_mem'

loop do
  10_000.times do
    pn = MiniPhone::PhoneNumber.new('+1 404 384 1384')
    pn.e164
    pn.valid?
  end

  memory_mb = GetProcessMem.new.mb

  puts '======'
  puts "Memory usage: #{memory_mb}mb"
  puts '======'

  break if ARGV.first == '--once'
end
