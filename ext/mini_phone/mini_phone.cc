#include "mini_phone.h"
#include "phonenumbers/phonenumberutil.h"

static VALUE rb_mMiniPhone;

static VALUE rb_is_phone_number_valid(VALUE self, VALUE str) {
  ::i18n::phonenumbers::PhoneNumber parsed_number;
  ::i18n::phonenumbers::PhoneNumberUtil* phone_util = ::i18n::phonenumbers::PhoneNumberUtil::GetInstance();

  std::string phone_number = RSTRING_PTR(str);

  auto result = phone_util->ParseAndKeepRawInput(phone_number, "US", &parsed_number);

  if (result == ::i18n::phonenumbers::PhoneNumberUtil::NO_PARSING_ERROR && phone_util->IsValidNumber(parsed_number)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE rb_format_phone_number(VALUE self, VALUE str, ::i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat format) {
  ::i18n::phonenumbers::PhoneNumber parsed_number;
  std::string formatted_number;
  ::i18n::phonenumbers::PhoneNumberUtil* phone_util = ::i18n::phonenumbers::PhoneNumberUtil::GetInstance();

  std::string phone_number = RSTRING_PTR(str);
  std::string country_code = RSTRING_PTR(rb_iv_get(self, "@default_country_code"));

  auto result = phone_util->ParseAndKeepRawInput(phone_number, country_code, &parsed_number);

  if (result == ::i18n::phonenumbers::PhoneNumberUtil::NO_PARSING_ERROR) {
    phone_util->Format(parsed_number, format, &formatted_number);
    return rb_str_new_cstr(formatted_number.c_str());
  } else {
    return Qnil;
  }
}

static VALUE rb_format_phone_number_e164(VALUE self, VALUE str) {
  return rb_format_phone_number(self, str, ::i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::E164);
}

static VALUE rb_format_phone_number_national(VALUE self, VALUE str) {
  return rb_format_phone_number(self, str, ::i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::NATIONAL);
}

static VALUE rb_format_phone_number_international(VALUE self, VALUE str) {
  return rb_format_phone_number(self, str, ::i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::INTERNATIONAL);
}

static VALUE rb_format_phone_number_rfc3966(VALUE self, VALUE str) {
  return rb_format_phone_number(self, str, ::i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::RFC3966);
}

static VALUE rb_set_default_country_code(VALUE self, VALUE str_code) {
  return rb_iv_set(self, "@default_country_code", str_code);
}

static VALUE rb_get_default_country_code(VALUE self) {
  return rb_iv_get(self, "@default_country_code");
}

extern "C" 
void Init_mini_phone(void)
{
  rb_mMiniPhone = rb_define_module("MiniPhone");

  rb_iv_set(rb_mMiniPhone, "@default_country_code", rb_str_new_cstr("US"));

  rb_define_module_function(rb_mMiniPhone, "valid?", rb_is_phone_number_valid, 1);
  rb_define_module_function(rb_mMiniPhone, "format_e164", rb_format_phone_number_e164, 1);
  rb_define_module_function(rb_mMiniPhone, "format_national", rb_format_phone_number_national, 1);
  rb_define_module_function(rb_mMiniPhone, "format_international", rb_format_phone_number_international, 1);
  rb_define_module_function(rb_mMiniPhone, "format_rfc3966", rb_format_phone_number_rfc3966, 1);
  rb_define_module_function(rb_mMiniPhone, "default_country_code=", rb_set_default_country_code, 1);
  rb_define_module_function(rb_mMiniPhone, "default_country_code", rb_get_default_country_code, 0);
}
