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
  end

  describe '.valid_for_country?' do
    it 'returns true for valid phone numbers' do
      expect(MiniPhone.valid_for_country?('4043841384', 'US')).to eq(true)
    end

    it 'returns false for invalid phone numbers' do
      expect(MiniPhone.valid_for_country?('-12', 'US')).to eq(false)
    end
  end

  describe '.invalid?' do
    it 'returns false for valid phone numbers' do
      expect(MiniPhone.invalid_for_country?('4043841384', 'US')).to eq(false)
    end

    it 'returns true for invalid phone numbers' do
      expect(MiniPhone.invalid_for_country?('-12', 'US')).to eq(true)
    end
  end

  describe '.possible?' do
    it 'returns true for possible phone numbers' do
      expect(MiniPhone.possible?('+16502530000')).to eq(true)
    end

    it 'returns false for invalid phone numbers' do
      expect(MiniPhone.possible?('-12')).to eq(false)
    end
  end

  describe '.impossible?' do
    it 'returns false for possible phone numbers' do
      expect(MiniPhone.impossible?('+16502530000')).to eq(false)
    end

    it 'returns true for impossible phone numbers' do
      expect(MiniPhone.impossible?('-12')).to eq(true)
    end
  end

  describe '.default_country_code=' do
    around do |ex|
      old = MiniPhone.default_country_code
      ex.run
    ensure
      MiniPhone.default_country_code = old
    end

    it 'configures the country code globally' do
      MiniPhone.default_country_code = 'NZ'

      expect(MiniPhone.valid?('4043841384')).to eq(false)
    end

    it 'defaults to unkown' do
      expect(MiniPhone.default_country_code).to eql('ZZ')
    end
  end
end
