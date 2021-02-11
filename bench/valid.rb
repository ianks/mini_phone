# frozen_string_literal: true

require 'bundler/setup'
require 'mini_phone'

Bundler.require(:bench)

Benchmark.ips do |x|
  x.report('MiniPhone: valid?') do
    MiniPhone.valid?('+14043841384')
    MiniPhone.valid?('444')
  end

  x.report('Phonelib: valid?') do
    Phonelib.valid?('+14043841384')
    Phonelib.valid?('444')
  end

  x.report('TelephoneNumber: valid?') do
    TelephoneNumber.valid?('+14043841384')
    TelephoneNumber.valid?('444')
  end

  x.compare!
end
