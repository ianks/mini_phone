# frozen_string_literal: true

RSpec.describe MiniPhone do
  it 'has a version number' do
    expect(MiniPhone::VERSION).not_to be nil
  end

  describe '.valid?' do
    it 'returns true for valid phone numbers' do
      expect(MiniPhone.valid?('+14043841384')).to eq(true)
    end

    it 'returns false for valid phone numbers' do
      expect(MiniPhone.valid?('-12')).to eq(false)
    end
  end

  describe '.format_e164' do
    it 'can format in e164' do
      expect(MiniPhone.format_e164('404-384-1384')).to eq('+14043841384')
    end
  end

  describe '.default_country_code' do
    it 'defaults to US' do
      expect(MiniPhone.default_country_code).to eq('US')
    end
  end
end
