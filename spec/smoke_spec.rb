# frozen_string_literal: true

RSpec.describe 'Smoke testing' do
  it 'does not leak memory' do
    require 'get_process_mem'

    # warmup
    10.times do
      10_000.times { MiniPhone::PhoneNumber.new('+1 404 384 1384') }
      GC.start
    end
    10.times { GC.start }

    100_000.times do
      MiniPhone::PhoneNumber.new('+1 404 384 1384')
    end
    10.times { GC.start }
    prev_bytes = GetProcessMem.new.mb

    100_000.times do
      MiniPhone::PhoneNumber.new('+1 404 384 1384')
    end
    10.times { GC.start }
    curr_bytes = GetProcessMem.new.mb

    expect(curr_bytes).to be_within(3).of(prev_bytes)
  end

  it 'does not cause any segfaults' do
    Thread.abort_on_exception = true

    threads = Array.new(10).map do
      Thread.new do |_t|
        Array.new(1000).map do
          pn = MiniPhone::PhoneNumber.new('+1 404 384 1384')
          pn.e164
          pn.country_code
          pn.valid?
          pn.type
          pn
        end

        GC.start
        GC.compact if GC.respond_to?(:compact)
      end
    end

    threads.map(&:join)
  end

  it 'can handle conccurent access' do
    Thread.abort_on_exception = true

    threads = Array.new(50).map do
      Thread.new do |_t|
        pn = MiniPhone::PhoneNumber.new('+1 404 384 1384')

        Array.new(100).map do
          pn.e164
          pn.country_code
          pn.valid?
          pn.type
          pn
        end
      end
    end

    threads.map(&:join)
  end
end
