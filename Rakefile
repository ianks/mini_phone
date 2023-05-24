# frozen_string_literal: true

require 'bundler/gem_tasks'
require 'rspec/core/rake_task'
require 'rubygems/package_task'
require 'rake/extensiontask'

RSpec::Core::RakeTask.new(:spec)

task default: %i[clobber compile spec lint]

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

task :lint do
  sh 'bundle exec rubocop'
end

task :format do
  sh 'bundle exec rubocop -A'
  sh 'clang-format -i ext/**/*.{h,cc}'
end

task deploy: :default do
  sh 'code -w ./lib/mini_phone/version.rb'
  version = `ruby -r ./lib/mini_phone/version.rb -e 'print MiniPhone::VERSION'`.strip
  sh "git commit -am 'Bump to v#{version} :confetti_ball:'"
  sh 'bundle exec rake release'
end

namespace :debug do
  desc 'Plot memory'
  task :memory do
    sh 'debug/memory_plot/plot.sh'
  end

  desc 'Run valgrind test'
  task :valgrind do
    sh 'docker build --tag mini_phone_dev -f Dockerfile.dev .'
    args = '--tool=memcheck --num-callers=15 --partial-loads-ok=yes --undef-value-errors=no'
    script = 'ruby debug/memory_plot/memory.rb --once'
    cmd = "docker run -it --rm -v #{Dir.pwd}:/app -w /app mini_phone_dev valgrind #{args} #{script}"
    puts cmd
    system cmd
  end
end
