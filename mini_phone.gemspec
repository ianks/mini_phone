# frozen_string_literal: true

require_relative 'lib/mini_phone/version'

Gem::Specification.new do |spec|
  spec.name          = 'mini_phone'
  spec.version       = MiniPhone::VERSION
  spec.authors       = ['Ian Ker-Seymer']
  spec.email         = ['i.kerseymer@gmail.com']

  spec.summary       = 'Uses the Google libphonenumber C lib to parse, validate, and format phone numbers'
  spec.description   = <<~MSG.strip
    Plugs directly in the the Google's native C++ [libphonenumber](https://github.com/google/libphonenumber) for extemely _fast_ and _robust_ phone number parsing, validation, and formatting. On average, most methods are 40x t0 50x faster than other Ruby phone number libraries.
  MSG
  spec.homepage      = 'https://github.com/ianks/mini_phone'
  spec.license       = 'MIT'
  spec.required_ruby_version = Gem::Requirement.new('>= 3.0.0')

  spec.platform = Gem::Platform::RUBY

  # spec.metadata["allowed_push_host"] = "TODO: Set to 'http://mygemserver.com'"

  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['source_code_uri'] = 'https://github.com/ianks/mini_phone'
  spec.metadata['changelog_uri'] = 'https://github.com/ianks/mini_phone/blob/master/CHANGELOG.md'

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`
      .split("\x0")
      .reject { |f| f.match(%r{^(bin|spec|bench)/}) }
      .reject { |f| f.start_with?('.') }
  end
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions    = ['ext/mini_phone/extconf.rb']

  # Security
  spec.metadata['rubygems_mfa_required'] = 'true'
  spec.cert_chain = ['certs/ianks.pem']
  spec.signing_key = File.expand_path('~/.ssh/gem-private_key.pem') if $PROGRAM_NAME.end_with?('gem')
end
