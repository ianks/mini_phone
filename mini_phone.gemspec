# frozen_string_literal: true

require_relative 'lib/mini_phone/version'

Gem::Specification.new do |spec|
  spec.name          = 'mini_phone'
  spec.version       = MiniPhone::VERSION
  spec.authors       = ['Ian Ker-Seymer']
  spec.email         = ['i.kerseymer@gmail.com']

  spec.summary       = 'Uses the Google libphonenumber C lib to parse, validate, and format phone numbers'
  spec.description   = spec.summary
  spec.homepage      = 'https://github.com/ianks/mini_phone'
  spec.license       = 'MIT'
  spec.required_ruby_version = Gem::Requirement.new('>= 2.5.0')

  # spec.metadata["allowed_push_host"] = "TODO: Set to 'http://mygemserver.com'"

  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['source_code_uri'] = 'https://github.com/ianks/mini_phone'
  spec.metadata['changelog_uri'] = 'https://github.com/ianks/mini_phone/blob/master/CHANGELOG.md'

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  end
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions    = ['ext/mini_phone/extconf.rb']
end
