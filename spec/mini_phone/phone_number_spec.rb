# frozen_string_literal: true

RSpec.describe MiniPhone::PhoneNumber do
  let(:valid_phone_number) { MiniPhone::PhoneNumber.new('+14043841384') }

  describe '#initialize' do
    it 'raises an argument error when an unparseable phone number is passed in' do
      expect do
        MiniPhone::PhoneNumber.new('aaaa')
      end.to raise_error(ArgumentError)
    end

    it 'raises an argument error when nil is passed in' do
      expect do
        MiniPhone::PhoneNumber.new(nil)
      end.to raise_error(TypeError)
    end

    it 'allows for a region code to be specified' do
      pn = MiniPhone::PhoneNumber.new('7911 123456', 'GB')

      expect(pn.region_code).to eql('GB')
    end
  end

  describe '#parse' do
    it 'instantiates a MiniPhone::PhoneNUmber' do
      pn = MiniPhone::PhoneNumber.parse('4043841384', 'US')

      expect(pn).to be_a(MiniPhone::PhoneNumber)
      expect(pn.region_code).to eql('US')
    end
  end

  shared_examples :memoization do |method_name|
    it 'memoizes' do
      expect(valid_phone_number.__send__(method_name)).to eql(valid_phone_number.__send__(method_name))
    end
  end

  describe '#valid?' do
    include_examples :memoization, :valid?

    it 'returns true for valid phone numbers' do
      expect(valid_phone_number.valid?).to eq(true)
    end

    it 'returns false for invalid phone numbers' do
      pn = MiniPhone::PhoneNumber.new('+9792423232345')

      expect(pn.valid?).to eq(false)
    end
  end

  describe '#e164' do
    include_examples :memoization, :e164

    it 'formats the number' do
      expect(valid_phone_number.e164).to eq('+14043841384')
    end
  end

  describe '#national' do
    include_examples :memoization, :national

    it 'formats the number' do
      expect(valid_phone_number.national).to eq('(404) 384-1384')
    end
  end

  describe '#international' do
    include_examples :memoization, :international

    it 'formats the number' do
      expect(valid_phone_number.international).to eq('+1 404-384-1384')
    end
  end

  describe '#rfc3966' do
    include_examples :memoization, :rfc3966

    it 'formats the number' do
      expect(valid_phone_number.rfc3966).to eq('tel:+1-404-384-1384')
    end
  end

  describe '#region_code' do
    it 'specifies the country code' do
      expect(valid_phone_number.region_code).to eq('US')
      expect(valid_phone_number.region_code).to eq('US')
    end
  end

  describe '#country_code' do
    it 'specifies the country code' do
      expect(valid_phone_number.country_code).to eq(1)
    end
  end

  describe '#type' do
    [
      ['+1 (800) 221-1212', :toll_free],
      ['+12423570000', :mobile],
      ['+12423651234', :fixed_line],
      ['+16502531111', :fixed_line_or_mobile],
      ['+445631231234', :voip],
      ['+447031231234', :personal_number],
      ['+165025311111', :unknown]
    ].each do |num, type|
      it "detects #{num.inspect} as #{type.inspect}" do
        pn = MiniPhone::PhoneNumber.new(num)

        expect(pn.type).to eq(type)
      end
    end

    it 'specifies the type for a toll free number' do
      pn = MiniPhone::PhoneNumber.new('+1 (800) 221-1212')

      expect(pn.type).to eq(:toll_free)
    end

    it 'specifies the type for a mobile number' do
      pn = MiniPhone::PhoneNumber.new('+12423570000')

      expect(pn.type).to eq(:mobile)
    end

    it 'specifies the type for a fixed line number' do
      pn = MiniPhone::PhoneNumber.new('+12423651234')

      expect(pn.type).to eq(:fixed_line)
    end
  end

  describe '#==' do
    it 'is equal to another number which is logically the same' do
      other = MiniPhone::PhoneNumber.new('+1 404 384 1384')
      expect(valid_phone_number).to eql(other)
    end

    it 'is not equal to another number which is logically different' do
      other = MiniPhone::PhoneNumber.new('+1 404 384 1385')
      expect(valid_phone_number).to eql(other)
    end
  end

  describe 'fuzz testing' do
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
          GC.compact
        end
      end

      threads.map(&:join)
    end
  end
end
