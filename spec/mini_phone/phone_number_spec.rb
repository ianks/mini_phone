# frozen_string_literal: true

RSpec.describe MiniPhone::PhoneNumber do
  let(:valid_phone_number) { MiniPhone::PhoneNumber.new('+14043841384') }

  describe '#initialize' do
    it 'allows invalid numbers' do
      expect do
        MiniPhone::PhoneNumber.new('aaaa')
      end.not_to raise_error
    end

    it 'does not raise an error with a nil phone number' do
      expect do
        MiniPhone::PhoneNumber.new(nil)
      end.not_to raise_error
    end

    it 'allows for a region code to be specified' do
      pn = MiniPhone::PhoneNumber.new('7911 123456', 'GB')

      expect(pn.region_code).to eql('GB')
    end

    it 'can be initialized with a fixnum' do
      pn = MiniPhone::PhoneNumber.new(4_043_841_384, 'US')

      expect(pn.e164).to eql('+14043841384')
    end

    it 'assumes unknown region code when initialized with nil' do
      pn = MiniPhone::PhoneNumber.parse('t', 'US')

      expect(pn.region_code).to eql(nil)
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

    it 'returns false for invalid phone numbers for a region' do
      pn = MiniPhone::PhoneNumber.new('+447975777666', 'US')

      expect(pn.valid?).to eq(false)
    end
  end

  describe '#invalid?' do
    it 'returns false for valid phone numbers' do
      expect(valid_phone_number.invalid?).to eq(false)
    end

    it 'returns true for invalid phone numbers' do
      pn = MiniPhone::PhoneNumber.new('+9792423232345')

      expect(pn.invalid?).to eq(true)
    end
  end

  describe '#possible?' do
    include_examples :memoization, :possible?

    it 'returns true for possible phone numbers' do
      expect(valid_phone_number.possible?).to eq(true)
    end

    it 'returns false for impossible phone numbers' do
      pn = MiniPhone::PhoneNumber.new('-12')

      expect(pn.possible?).to eq(false)
    end
  end

  describe '#impossible?' do
    it 'returns true for possible phone numbers' do
      expect(valid_phone_number.impossible?).to eq(false)
    end

    it 'returns false for impossible phone numbers' do
      pn = MiniPhone::PhoneNumber.new('-12')

      expect(pn.impossible?).to eq(true)
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

  describe '#raw_national' do
    include_examples :memoization, :raw_national

    it 'formats the number' do
      expect(valid_phone_number.raw_national).to eq('4043841384')
    end
  end

  describe '#raw_international' do
    include_examples :memoization, :raw_international

    it 'formats the number' do
      expect(valid_phone_number.raw_international).to eq('14043841384')
    end
  end

  describe '#dasherized_national' do
    include_examples :memoization, :dasherized_national

    it 'formats the number' do
      expect(valid_phone_number.dasherized_national).to eq('404-384-1384')
    end
  end

  describe '#dasherized_international' do
    include_examples :memoization, :dasherized_international

    it 'formats the number' do
      expect(valid_phone_number.dasherized_international).to eq('1-404-384-1384')
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
    include_examples :memoization, :region_code

    it 'specifies the country code' do
      expect(valid_phone_number.region_code).to eq('US')
    end
  end

  describe '#country' do
    include_examples :memoization, :country

    it 'specifies the country code' do
      expect(valid_phone_number.country).to eq('US')
    end
  end

  describe '#country_code' do
    include_examples :memoization, :country_code

    it 'specifies the country code' do
      expect(valid_phone_number.country_code).to eq(1)
    end
  end

  describe '#area_code' do
    include_examples :memoization, :area_code

    it 'specifies the country code' do
      expect(valid_phone_number.area_code).to eq('404')
    end
  end

  describe '#to_s' do
    it 'returns the input' do
      expect(valid_phone_number.to_s).to eq('+14043841384')
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

      expect(valid_phone_number).to eq(other)
    end

    it 'is not equal to another number which is logically different' do
      other = MiniPhone::PhoneNumber.new('+1 404 384 1385')

      expect(valid_phone_number).not_to eq other
    end

    it 'is eql when parsing directly' do
      expect(MiniPhone.parse('+14043841384')).to eq(MiniPhone.parse('4043841384', 'US'))
    end

    it 'works with nil' do
      expect(MiniPhone::PhoneNumber.new(nil)).to eq(MiniPhone::PhoneNumber.new(nil))
    end
  end

  context 'with a nil phone number' do
    (MiniPhone::PhoneNumber.instance_methods(false) - %i[valid? eql? == valid? invalid? possible?
                                                         impossible?]).each do |method_name|
      describe method_name do
        it 'returns nil' do
          pn = MiniPhone::PhoneNumber.new(nil)

          expect(pn.__send__(method_name)).to be_nil
        end
      end
    end
  end

  it 'has an accurate mem_size' do
    require 'objspace'

    pn = MiniPhone::PhoneNumber.new('+1 404 384 11111111111111')

    expect(ObjectSpace.memsize_of(pn)).to eql(192)
  end
end
