# frozen_string_literal: true

require 'bundler/gem_tasks'
require 'rspec/core/rake_task'
require 'rubygems/package_task'
require 'rake/extensiontask'

RSpec::Core::RakeTask.new(:spec)

task build: :compile

task default: %i[clobber compile spec]

spec = Gem::Specification.load(File.expand_path('mini_phone.gemspec', __dir__))

Gem::PackageTask.new(spec) do |pkg|
  pkg.need_zip = true
  pkg.need_tar = true
end

Rake::ExtensionTask.new('mini_phone', spec) do |ext|
  ext.lib_dir = 'lib/mini_phone'
end

task bench: %i[clobber compile] do
  Dir['bench/**/*'].each do |f|
    require_relative f
  end
end
