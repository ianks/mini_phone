# frozen_string_literal: true

RSpec.describe MiniPhone do
  it 'has a version number' do
    expect(MiniPhone::VERSION).not_to be nil
  end

  describe '.valid?' do
    it 'returns true for valid phone numbers' do
      expect(MiniPhone.valid?('+14043841384')).to eq(true)
    end

    it 'returns false for invalid phone numbers' do
      expect(MiniPhone.valid?('-12')).to eq(false)
    end

    it 'handles nil' do
      expect(MiniPhone.valid?(nil)).to eq(false)
    end
  end

  describe '.parse' do
    it 'returns a MiniPhone::PhoneNumber instance' do
      pn = MiniPhone.parse('+14043841384')

      expect(pn).to respond_to(:e164, :country_code)
    end

    it 'accepts a country code' do
      pn = MiniPhone.parse('4043841384', 'US')

      expect(pn).to respond_to(:e164, :country_code)
    end
  end

  describe '.valid_for_country?' do
    it 'returns true for valid phone numbers' do
      expect(MiniPhone.valid_for_country?('4043841384', 'US')).to eq(true)
    end

    it 'returns false for invalid phone numbers' do
      expect(MiniPhone.valid_for_country?('-12', 'US')).to eq(false)
    end

    it 'returns false for numbers not in a region' do
      result = MiniPhone.valid_for_country?('+44 1434 634996', 'US')

      expect(result).to eql(false)
    end

    it 'returns true for numbers in a valid region' do
      result = MiniPhone.valid_for_country?('+44 1434 634996', 'GB')

      expect(result).to eql(true)
    end
  end

  describe '.invalid_for_country?' do
    it 'returns false for valid phone numbers' do
      expect(MiniPhone.invalid_for_country?('4043841384', 'US')).to eq(false)
    end

    it 'returns true for invalid phone numbers' do
      expect(MiniPhone.invalid_for_country?('-12', 'US')).to eq(true)
    end

    it 'returns true for numbers not in a region' do
      result = MiniPhone.invalid_for_country?('+44 1434 634996', 'US')

      expect(result).to eql(true)
    end

    it 'returns false for numbers in a valid region' do
      result = MiniPhone.invalid_for_country?('+44 1434 634996', 'GB')

      expect(result).to eql(false)
    end
  end

  describe '.possible?' do
    it 'returns true for possible phone numbers' do
      expect(MiniPhone.possible?('+16502530000')).to eq(true)
    end

    it 'returns false for invalid phone numbers' do
      expect(MiniPhone.possible?('-12')).to eq(false)
    end

    it 'handles nil' do
      expect(MiniPhone.possible?(nil)).to eq(false)
    end
  end

  describe '.impossible?' do
    it 'returns false for possible phone numbers' do
      expect(MiniPhone.impossible?('+16502530000')).to eq(false)
    end

    it 'returns true for impossible phone numbers' do
      expect(MiniPhone.impossible?('-12')).to eq(true)
    end

    it 'handles nil' do
      expect(MiniPhone.impossible?(nil)).to eq(true)
    end
  end

  describe '.default_country=' do
    around do |ex|
      old = MiniPhone.default_country
      ex.run
    ensure
      MiniPhone.default_country = old
    end

    it 'configures the country code globally' do
      MiniPhone.default_country = 'NZ'

      expect(MiniPhone.valid?('4043841384')).to eq(false)
    end

    it 'defaults to unkown' do
      expect(MiniPhone.default_country).to eql('ZZ')
    end

    it 'handles nil' do
      MiniPhone.default_country = nil

      expect(MiniPhone.default_country).to eql('ZZ')
    end
  end

  describe '.normalize_digits_only' do
    it 'handles nil values' do
      expect(MiniPhone.normalize_digits_only(nil)).to eq(nil)
    end

    it 'normalizes phone numbers' do
      expect(MiniPhone.normalize_digits_only('034-56&+a#234')).to eq('03456234')
    end
  end
end
