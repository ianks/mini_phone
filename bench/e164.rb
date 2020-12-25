# frozen_string_literal: true

require 'bundler/setup'
require 'mini_phone'

Bundler.require(:bench)

Benchmark.ips do |x|
  x.report('MiniPhone: e164') do
    pn = MiniPhone::PhoneNumber.new('+1 404-384-1384')
    pn.e164
  end

  x.report('Phonelib: e164') do
    pn = Phonelib.parse('+1 404-384-1384')
    pn.e164
  end

  x.compare!
end
