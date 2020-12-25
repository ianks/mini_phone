# frozen_string_literal: true

require 'bundler/gem_tasks'
require 'rspec/core/rake_task'

RSpec::Core::RakeTask.new(:spec)

require 'rake/extensiontask'

task build: :compile

Rake::ExtensionTask.new('mini_phone') do |ext|
  ext.lib_dir = 'lib/mini_phone'
end

task default: %i[clobber compile spec]

task bench: %i[clobber compile] do
  Dir['bench/**/*'].each do |f|
    require_relative f
  end
end
