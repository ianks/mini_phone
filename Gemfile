# frozen_string_literal: true

source 'https://rubygems.org'

# Specify your gem's dependencies in mini_phone.gemspec
gemspec

gem 'rake', '~> 12.0'
gem 'rake-compiler', github: 'larskanis/rake-compiler', branch: 'fix-native-version'
gem 'rspec', '~> 3.0'
gem 'rspec-github', require: false
gem 'rubocop'

group :bench do
  gem 'benchmark-ips'
  gem 'phonelib'
end
