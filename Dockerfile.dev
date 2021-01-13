FROM ruby:2.7-alpine

WORKDiR /app

# RUN apt-get update -y && apt-get install -y valgrind libphonenumber
RUN apk add --no-cache libphonenumber-dev valgrind git make libffi-dev build-base
COPY Gemfile Gemfile.lock mini_phone.gemspec ./
COPY lib/mini_phone/version.rb ./lib/mini_phone/version.rb
RUN bundle install -j4