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

namespace :publish do
  def push_to_github_registry(gem)
    puts "Pushing #{gem} to GitHub Package Registry"
    sh "gem push #{gem} --host https://rubygems.pkg.github.com/ianks --key github"
  end

  def push_to_rubygems(gem)
    puts "Pushing #{gem} to Rubygems"
    sh "gem push #{gem}"
  end

  task native: %i[native gem] do
    g = "./pkg/mini_phone-#{MiniPhone::VERSION}-#{Gem::Platform.new(RUBY_PLATFORM)}.gem"

    push_to_github_registry(g)
    push_to_rubygems(g)
  end

  task non_native: [:gem] do
    g = "./pkg/mini_phone-#{MiniPhone::VERSION}.gem"

    push_to_github_registry(g)
    push_to_rubygems(g)
  end
end
