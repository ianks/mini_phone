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
end
