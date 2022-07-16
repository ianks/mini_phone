# frozen_string_literal: true

source 'https://rubygems.org'

# Specify your gem's dependencies in mini_phone.gemspec
gemspec

gem 'rake', require: false
# https://github.com/rake-compiler/rake-compiler/pull/166
gem 'get_process_mem', require: false
gem 'pry', require: false
gem 'rake-compiler' # , github: 'larskanis/rake-compiler', branch: 'fix-native-version'
gem 'rspec', '~> 3.0', require: false
gem 'rspec-github', require: false
gem 'rubocop', require: false

group :bench do
  gem 'benchmark-ips'
  gem 'phonelib'
  gem 'telephone_number'
end
