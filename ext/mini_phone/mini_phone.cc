#include "mini_phone.h"
#include "phonenumbers/phonenumberutil.h"

using namespace ::i18n::phonenumbers;

static VALUE rb_mMiniPhone;

static VALUE rb_cPhoneNumber;

extern "C" struct PhoneNumberInfo {
  PhoneNumber phone_number;
  std::string raw_phone_number;
  std::string raw_country_code;
};

static inline VALUE is_phone_number_valid(VALUE self, VALUE str, VALUE cc) {
  PhoneNumber parsed_number;
  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();

  std::string phone_number(RSTRING_PTR(str), RSTRING_LEN(str));
  std::string country_code(RSTRING_PTR(cc), RSTRING_LEN(cc));

  auto result = phone_util->ParseAndKeepRawInput(phone_number, country_code, &parsed_number);

  if (result == PhoneNumberUtil::NO_PARSING_ERROR && phone_util->IsValidNumber(parsed_number)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

extern "C" VALUE rb_is_phone_number_valid(VALUE self, VALUE str) {
  VALUE def_cc = rb_iv_get(rb_mMiniPhone, "@default_country_code");

  return is_phone_number_valid(self, str, def_cc);
}

extern "C" VALUE rb_is_phone_number_valid_for_country(VALUE self, VALUE str, VALUE cc) {
  return is_phone_number_valid(self, str, cc);
}

extern "C" VALUE rb_is_phone_number_invalid(VALUE self, VALUE str) {
  return rb_is_phone_number_valid(self, str) == Qtrue ? Qfalse : Qtrue;
}

extern "C" VALUE rb_is_phone_number_invalid_for_country(VALUE self, VALUE str, VALUE cc) {
  return is_phone_number_valid(self, str, cc) == Qtrue ? Qfalse : Qtrue;
}

extern "C" VALUE rb_is_phone_number_possible(VALUE self, VALUE str) {
  PhoneNumber parsed_number;
  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();

  VALUE def_cc = rb_iv_get(rb_mMiniPhone, "@default_country_code");
  std::string phone_number(RSTRING_PTR(str), RSTRING_LEN(str));
  std::string country_code(RSTRING_PTR(def_cc), RSTRING_LEN(def_cc));

  auto result = phone_util->Parse(phone_number, country_code, &parsed_number);

  if (result == PhoneNumberUtil::NO_PARSING_ERROR && phone_util->IsPossibleNumber(parsed_number)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

extern "C" VALUE rb_is_phone_number_impossible(VALUE self, VALUE str) {
  return rb_is_phone_number_possible(self, str) == Qtrue ? Qfalse : Qtrue;
}

extern "C" VALUE rb_set_default_country_code(VALUE self, VALUE str_code) {
  return rb_iv_set(self, "@default_country_code", str_code);
}

extern "C" VALUE rb_get_default_country_code(VALUE self) { return rb_iv_get(self, "@default_country_code"); }

extern "C" void rb_phone_number_dealloc(PhoneNumberInfo *phone_number_info) { delete phone_number_info; }

extern "C" VALUE rb_phone_number_parse(int argc, VALUE *argv, VALUE self) {
  return rb_class_new_instance(argc, argv, rb_cPhoneNumber);
}

extern "C" VALUE rb_phone_number_alloc(VALUE self) {
  PhoneNumberInfo *phone_number_info = new PhoneNumberInfo();

  /* wrap */
  return Data_Wrap_Struct(self, NULL, &rb_phone_number_dealloc, phone_number_info);
}

static inline VALUE rb_phone_number_nullify_ivars(VALUE self) {
  rb_iv_set(self, "@national", Qnil);
  rb_iv_set(self, "@international", Qnil);
  rb_iv_set(self, "@e164", Qnil);
  rb_iv_set(self, "@country_code", Qnil);
  rb_iv_set(self, "@region_code", Qnil);
  rb_iv_set(self, "@rfc3966", Qnil);
  rb_iv_set(self, "@type", Qnil);
  rb_iv_set(self, "@valid", Qfalse);
  rb_iv_set(self, "@possible", Qfalse);

  return Qtrue;
}

extern "C" VALUE rb_phone_number_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE str;
  VALUE def_cc;

  rb_scan_args(argc, argv, "11", &str, &def_cc);

  if (NIL_P(def_cc)) {
    def_cc = rb_iv_get(rb_mMiniPhone, "@default_country_code");
  }

  rb_iv_set(self, "@input", str);

  if (FIXNUM_P(str)) {
    str = rb_fix2str(str, 10);
  } else if (!RB_TYPE_P(str, T_STRING)) {
    return rb_phone_number_nullify_ivars(self);
  }

  PhoneNumberInfo *phone_number_info;
  PhoneNumber parsed_number;

  Data_Get_Struct(self, PhoneNumberInfo, phone_number_info);

  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();

  std::string phone_number(RSTRING_PTR(str), RSTRING_LEN(str));
  std::string country_code(RSTRING_PTR(def_cc), RSTRING_LEN(def_cc));

  auto result = phone_util->Parse(phone_number, country_code, &parsed_number);

  if (result != PhoneNumberUtil::NO_PARSING_ERROR) {
    rb_phone_number_nullify_ivars(self);
  } else {
    phone_number_info->phone_number = parsed_number;
  }

  return self;
}

static inline VALUE rb_phone_number_format(VALUE self, PhoneNumberUtil::PhoneNumberFormat fmt) {
  std::string formatted_number;
  PhoneNumberInfo *phone_number_info;
  Data_Get_Struct(self, PhoneNumberInfo, phone_number_info);

  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();
  PhoneNumber parsed_number = phone_number_info->phone_number;
  phone_util->Format(parsed_number, fmt, &formatted_number);

  return rb_str_new(formatted_number.c_str(), formatted_number.size());
}

extern "C" VALUE rb_phone_number_e164(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@e164"))) {
    return rb_iv_get(self, "@e164");
  }

  return rb_iv_set(self, "@e164", rb_phone_number_format(self, PhoneNumberUtil::PhoneNumberFormat::E164));
}

extern "C" VALUE rb_phone_number_national(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@national"))) {
    return rb_iv_get(self, "@national");
  }

  return rb_iv_set(self, "@national", rb_phone_number_format(self, PhoneNumberUtil::PhoneNumberFormat::NATIONAL));
}

extern "C" VALUE rb_phone_number_international(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@international"))) {
    return rb_iv_get(self, "@international");
  }

  return rb_iv_set(self, "@international",
                   rb_phone_number_format(self, PhoneNumberUtil::PhoneNumberFormat::INTERNATIONAL));
}

extern "C" VALUE rb_phone_number_rfc3966(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@rfc3966"))) {
    return rb_iv_get(self, "@rfc3966");
  }

  return rb_iv_set(self, "@rfc3966", rb_phone_number_format(self, PhoneNumberUtil::PhoneNumberFormat::RFC3966));
}

extern "C" VALUE rb_phone_number_valid_eh(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@valid"))) {
    return rb_iv_get(self, "@valid");
  }

  std::string formatted_number;
  PhoneNumberInfo *phone_number_info;
  Data_Get_Struct(self, PhoneNumberInfo, phone_number_info);

  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();

  if (phone_util->IsValidNumber(phone_number_info->phone_number)) {
    return rb_iv_set(self, "@valid", Qtrue);
  } else {
    return rb_iv_set(self, "@valid", Qfalse);
  }
}

extern "C" VALUE rb_phone_number_invalid_eh(VALUE self) {
  return rb_phone_number_valid_eh(self) == Qtrue ? Qfalse : Qtrue;
}

extern "C" VALUE rb_phone_number_possible_eh(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@possible"))) {
    return rb_iv_get(self, "@possible");
  }

  std::string formatted_number;
  PhoneNumberInfo *phone_number_info;
  Data_Get_Struct(self, PhoneNumberInfo, phone_number_info);

  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();

  if (phone_util->IsPossibleNumber(phone_number_info->phone_number)) {
    return rb_iv_set(self, "@possible", Qtrue);
  } else {
    return rb_iv_set(self, "@possible", Qfalse);
  }
}

extern "C" VALUE rb_phone_number_impossible_eh(VALUE self) {
  return rb_phone_number_possible_eh(self) == Qtrue ? Qfalse : Qtrue;
}

extern "C" VALUE rb_phone_number_region_code(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@region_code"))) {
    return rb_iv_get(self, "@region_code");
  }

  PhoneNumberInfo *phone_number_info;
  std::string code;
  Data_Get_Struct(self, PhoneNumberInfo, phone_number_info);
  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();

  phone_util->GetRegionCodeForCountryCode(phone_number_info->phone_number.country_code(), &code);

  VALUE result = rb_str_new(code.c_str(), code.size());

  return rb_iv_set(self, "@region_code", result);
}

extern "C" VALUE rb_phone_number_country_code(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@country_code"))) {
    return rb_iv_get(self, "@country_code");
  }

  PhoneNumberInfo *phone_number_info;
  Data_Get_Struct(self, PhoneNumberInfo, phone_number_info);

  int code = phone_number_info->phone_number.country_code();

  VALUE result = INT2NUM(code);

  return rb_iv_set(self, "@country_code", result);
}

extern "C" VALUE rb_phone_number_eql_eh(VALUE self, VALUE other) {
  if (!rb_obj_is_instance_of(other, rb_cPhoneNumber)) {
    return Qfalse;
  }

  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();

  PhoneNumberInfo *self_phone_number_info;
  Data_Get_Struct(self, PhoneNumberInfo, self_phone_number_info);

  PhoneNumberInfo *other_phone_number_info;
  Data_Get_Struct(other, PhoneNumberInfo, other_phone_number_info);
  if (phone_util->IsNumberMatch(other_phone_number_info->phone_number, self_phone_number_info->phone_number)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

extern "C" VALUE rb_phone_number_type(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@type"))) {
    return rb_iv_get(self, "@type");
  }

  PhoneNumberInfo *phone_number_info;
  Data_Get_Struct(self, PhoneNumberInfo, phone_number_info);
  PhoneNumberUtil *phone_util = PhoneNumberUtil::GetInstance();

  VALUE result;

  // @see
  // https://github.com/google/libphonenumber/blob/4e9954edea7cf263532c5dd3861a801104c3f012/cpp/src/phonenumbers/phonenumberutil.h#L91
  switch (phone_util->GetNumberType(phone_number_info->phone_number)) {
  case PhoneNumberUtil::PREMIUM_RATE:
    result = rb_intern("premium_rate");
    break;
  case PhoneNumberUtil::TOLL_FREE:
    result = rb_intern("toll_free");
    break;
  case PhoneNumberUtil::MOBILE:
    result = rb_intern("mobile");
    break;
  case PhoneNumberUtil::FIXED_LINE:
    result = rb_intern("fixed_line");
    break;
  case PhoneNumberUtil::FIXED_LINE_OR_MOBILE:
    result = rb_intern("fixed_line_or_mobile");
    break;
  case PhoneNumberUtil::SHARED_COST:
    result = rb_intern("shared_cost");
    break;
  case PhoneNumberUtil::VOIP:
    result = rb_intern("voip");
    break;
  case PhoneNumberUtil::PERSONAL_NUMBER:
    result = rb_intern("personal_number");
    break;
  case PhoneNumberUtil::PAGER:
    result = rb_intern("pager");
    break;
  case PhoneNumberUtil::UAN:
    result = rb_intern("uan");
    break;
  case PhoneNumberUtil::VOICEMAIL:
    result = rb_intern("voicemail");
    break;
  default:
    result = rb_intern("unknown");
    break;
  }

  return rb_iv_set(self, "@type", ID2SYM(result));
}

extern "C" void Init_mini_phone(void) {
  rb_mMiniPhone = rb_define_module("MiniPhone");

  // Unknown
  rb_iv_set(rb_mMiniPhone, "@default_country_code", rb_str_new("ZZ", 2));

  rb_define_module_function(rb_mMiniPhone, "valid?", reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_valid), 1);
  rb_define_module_function(rb_mMiniPhone, "valid_for_country?",
                            reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_valid_for_country), 2);
  rb_define_module_function(rb_mMiniPhone, "invalid?", reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_invalid), 1);
  rb_define_module_function(rb_mMiniPhone, "invalid_for_country?",
                            reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_invalid_for_country), 2);
  rb_define_module_function(rb_mMiniPhone, "possible?", reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_valid), 1);
  rb_define_module_function(rb_mMiniPhone, "impossible?", reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_invalid),
                            1);
  rb_define_module_function(rb_mMiniPhone,
                            "default_country_code=", reinterpret_cast<VALUE (*)(...)>(rb_set_default_country_code), 1);
  rb_define_module_function(rb_mMiniPhone, "default_country_code",
                            reinterpret_cast<VALUE (*)(...)>(rb_get_default_country_code), 0);
  rb_define_module_function(rb_mMiniPhone, "parse", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_parse), -1);

  rb_cPhoneNumber = rb_define_class_under(rb_mMiniPhone, "PhoneNumber", rb_cObject);

  rb_define_singleton_method(rb_cPhoneNumber, "parse", reinterpret_cast<VALUE (*)(...)>(rb_class_new_instance), -1);
  rb_define_alloc_func(rb_cPhoneNumber, rb_phone_number_alloc);
  rb_define_method(rb_cPhoneNumber, "initialize", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_initialize), -1);
  rb_define_method(rb_cPhoneNumber, "valid?", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_valid_eh), 0);
  rb_define_method(rb_cPhoneNumber, "invalid?", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_invalid_eh), 0);
  rb_define_method(rb_cPhoneNumber, "possible?", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_possible_eh), 0);
  rb_define_method(rb_cPhoneNumber, "impossible?", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_impossible_eh), 0);
  rb_define_method(rb_cPhoneNumber, "e164", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_e164), 0);
  rb_define_method(rb_cPhoneNumber, "national", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_national), 0);
  rb_define_method(rb_cPhoneNumber, "international", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_international),
                   0);
  rb_define_method(rb_cPhoneNumber, "rfc3966", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_rfc3966), 0);
  rb_define_method(rb_cPhoneNumber, "region_code", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_region_code), 0);
  rb_define_method(rb_cPhoneNumber, "country_code", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_country_code), 0);
  rb_define_method(rb_cPhoneNumber, "type", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_type), 0);
  rb_define_method(rb_cPhoneNumber, "eql?", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_eql_eh), 1);
}
