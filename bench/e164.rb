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

  x.report('TelephoneNumber: e164') do
    pn = TelephoneNumber.parse("+1 404-384-1384")
    pn.e164_number
  end

  x.compare!
end
