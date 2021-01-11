# MiniPhone

A Ruby gem which plugs directly into Google's native C++
[libphonenumber](https://github.com/google/libphonenumber) for extremely
_fast_ and _robust_ phone number parsing, validation, and formatting. On
average, most methods are 40x to 50x faster than other Ruby phone number
libraries.

## Usage

### Checking if a phone number is valid

```ruby
MiniPhone.valid?('+14043841399')                     # true
MiniPhone.valid_for_country?('(404) 384-1399', 'US') # true
MiniPhone.valid_for_country?('(404) 384-1399', 'GB') # false
```

### Formatting a number

```ruby
MiniPhone.default_country = 'US'

phone_number = MiniPhone.parse('404-384-1399')

phone_number.e164          # +14043841399
phone_number.national      # (404) 384-1399
phone_number.international # +1 404-384-1399
phone_number.rfc3966       # tel:+1-404-384-1384
```

### Checking if a phone number is possible

```ruby
phone_number = MiniPhone.parse('-12')

phone_number.possible? # false
```

### Getting the type of a phone number

```ruby
MiniPhone.parse('+12423570000').type # :mobile
MiniPhone.parse('+12423651234').type # :fixed_line
```

The possible types are directly mapped from [this
enum](https://github.com/google/libphonenumber/blob/4e9954edea7cf263532c5dd3861a801104c3f012/cpp/src/phonenumbers/phonenumberutil.h#L91):

```ruby
:fixed_line
:mobile
:fixed_line_or_mobile
:toll_free
:premium_rate
:shared_cost
:voip
:personal_number
:pager
:uan
:voicemail
:unknown
```

## Compatibility with PhoneLib

MiniPhone aims to be compatible with
[Phonelib](https://github.com/daddyz/phonelib). In many cases it can be a
drop in replacement. It has a smaller feature set, so it is not a
replacement for every use case. If there is a feature you need, open
an issue and we will try to support it.

## Benchmarks

On average, most methods are 40x to 50x faster than other libraries. To run
the benchmarks locally, execute: `bundle exec rake bench`

```
Comparison:
   MiniPhone: valid?:    23111.4 i/s
    Phonelib: valid?:      482.3 i/s - 47.92x  (± 0.00) slower
```

```
Comparison:
     MiniPhone: e164:    31567.0 i/s
      Phonelib: e164:      652.3 i/s - 48.39x  (± 0.00) slower
```

## Installation

1. Install libphonenumber

   ```sh
   brew install libphonenumber # mac
   ```

   ```sh
   apt-get install -y libphonenumber # debian / ubuntu
   ```

2. Add this line to your application's Gemfile:

   ```ruby
   gem 'mini_phone'
   ```

3. And then execute:

   ```sh
   bundle install
   ```

   Or install it yourself as:

   ```sh
   gem install mini_phone
   ```

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then,
run `rake spec` to run the tests. You can also run `bin/console` for an
interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`.
To release a new version, update the version number in `version.rb`, and then
run `bundle exec rake release`, which will create a git tag for the version,
push git commits and tags, and push the `.gem` file to
[rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at
https://github.com/ianks/mini_phone. This project is intended to be a
safe, welcoming space for collaboration, and contributors are expected to
adhere to the [code of
conduct](https://github.com/ianks/mini_phone/blob/master/CODE_OF_CONDUCT.md).

## License

The gem is available as open source under the terms of the [MIT
License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the MiniPhone project's codebases, issue trackers,
chat rooms and mailing lists is expected to follow the [code of
conduct](https://github.com/ianks/mini_phone/blob/master/CODE_OF_CONDUCT.md).
