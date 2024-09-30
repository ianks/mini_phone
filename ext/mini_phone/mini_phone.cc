#include "mini_phone.h"
#include "phonenumbers/phonemetadata.pb.h"
#include "phonenumbers/phonenumber.pb.h"
#include "phonenumbers/phonenumberutil.h"

using namespace ::i18n::phonenumbers;

using google::protobuf::RepeatedPtrField;

static VALUE rb_mMiniPhone;

static VALUE rb_cPhoneNumber;

static RepeatedPtrField<NumberFormat> raw_national_format;
static RepeatedPtrField<NumberFormat> dasherized_national_format;

extern "C" struct PhoneNumberInfo {
  PhoneNumber *phone_number;
};

extern "C" size_t phone_number_info_size(const void *data) { return sizeof(PhoneNumberInfo); }

extern "C" void phone_number_info_free(void *data) {
  PhoneNumberInfo *phone_number_info = static_cast<PhoneNumberInfo *>(data);
  phone_number_info->phone_number->~PhoneNumber();
  xfree(phone_number_info->phone_number);
  phone_number_info->~PhoneNumberInfo();
  xfree(data);
}

extern "C" const rb_data_type_t phone_number_info_type = {
    .wrap_struct_name = "MiniPhone/PhoneNumberInfo",
    .function =
        {
            .dmark = NULL,
            .dfree = phone_number_info_free,
            .dsize = phone_number_info_size,
        },
    .parent = NULL,
    .data = NULL,
    .flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

static inline VALUE is_phone_number_valid(VALUE self, VALUE str, VALUE cc) {
  if (NIL_P(str) || NIL_P(cc)) {
    return Qfalse;
  }

  PhoneNumber parsed_number;
  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

  std::string phone_number(RSTRING_PTR(str), RSTRING_LEN(str));
  std::string country_code(RSTRING_PTR(cc), RSTRING_LEN(cc));

  auto result = phone_util.ParseAndKeepRawInput(phone_number, country_code, &parsed_number);

  if (result != PhoneNumberUtil::NO_PARSING_ERROR) {
    return Qfalse;
  }

  if (country_code == "ZZ" && phone_util.IsValidNumber(parsed_number)) {
    return Qtrue;
  } else if (phone_util.IsValidNumberForRegion(parsed_number, country_code)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

extern "C" VALUE rb_is_phone_number_valid(VALUE self, VALUE str) {
  VALUE input_region_code = rb_iv_get(rb_mMiniPhone, "@default_country");

  return is_phone_number_valid(self, str, input_region_code);
}

extern "C" VALUE rb_normalize_digits_only(VALUE self, VALUE str) {
  if (NIL_P(str)) {
    return Qnil;
  }

  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

  std::string phone_number(RSTRING_PTR(str), RSTRING_LEN(str));

  phone_util.NormalizeDigitsOnly(&phone_number);

  return rb_str_new(phone_number.c_str(), phone_number.size());
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
  if (NIL_P(str)) {
    return Qnil;
  }

  PhoneNumber parsed_number;
  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

  VALUE input_region_code = rb_iv_get(rb_mMiniPhone, "@default_country");
  std::string phone_number(RSTRING_PTR(str), RSTRING_LEN(str));
  std::string country_code(RSTRING_PTR(input_region_code), RSTRING_LEN(input_region_code));

  auto result = phone_util.Parse(phone_number, country_code, &parsed_number);

  if (result == PhoneNumberUtil::NO_PARSING_ERROR && phone_util.IsPossibleNumber(parsed_number)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

extern "C" VALUE rb_is_phone_number_impossible(VALUE self, VALUE str) {
  return rb_is_phone_number_possible(self, str) == Qtrue ? Qfalse : Qtrue;
}

extern "C" VALUE rb_set_default_country(VALUE self, VALUE str_code) {
  if (NIL_P(str_code)) {
    str_code = rb_str_new("ZZ", 2);
  }

  return rb_iv_set(self, "@default_country", str_code);
}

extern "C" VALUE rb_get_default_country(VALUE self) { return rb_iv_get(self, "@default_country"); }

extern "C" VALUE rb_phone_number_parse(int argc, VALUE *argv, VALUE self) {
  return rb_class_new_instance(argc, argv, rb_cPhoneNumber);
}

extern "C" VALUE rb_phone_number_alloc(VALUE self) {
  void *phone_number_data = ALLOC(PhoneNumber);
  void *data = ALLOC(PhoneNumberInfo);
  PhoneNumberInfo *phone_number_info = new (data) PhoneNumberInfo();
  PhoneNumber *phone_number = new (phone_number_data) PhoneNumber();
  phone_number_info->phone_number = phone_number;

  return TypedData_Wrap_Struct(self, &phone_number_info_type, phone_number_info);
}

static inline VALUE rb_phone_number_nullify_ivars(VALUE self) {
  rb_iv_set(self, "@national", Qnil);
  rb_iv_set(self, "@raw_national", Qnil);
  rb_iv_set(self, "@dasherized_national", Qnil);
  rb_iv_set(self, "@international", Qnil);
  rb_iv_set(self, "@raw_international", Qnil);
  rb_iv_set(self, "@dasherized_international", Qnil);
  rb_iv_set(self, "@e164", Qnil);
  rb_iv_set(self, "@country_code", Qnil);
  rb_iv_set(self, "@region_code", Qnil);
  rb_iv_set(self, "@rfc3966", Qnil);
  rb_iv_set(self, "@type", Qnil);
  rb_iv_set(self, "@valid", Qfalse);
  rb_iv_set(self, "@possible", Qfalse);
  rb_iv_set(self, "@area_code", Qnil);

  return Qtrue;
}

static inline VALUE rb_phone_number_format(VALUE self, PhoneNumberUtil::PhoneNumberFormat fmt) {
  std::string formatted_number;
  PhoneNumberInfo *phone_number_info;
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);

  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());
  PhoneNumber *parsed_number = phone_number_info->phone_number;
  phone_util.Format(*parsed_number, fmt, &formatted_number);

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

VALUE format_by_pattern_national(VALUE self, RepeatedPtrField<NumberFormat> format) {
  std::string formatted_number;
  PhoneNumberInfo *phone_number_info;
  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);

  phone_util.FormatByPattern(*phone_number_info->phone_number, PhoneNumberUtil::NATIONAL, format, &formatted_number);

  return rb_str_new(formatted_number.c_str(), formatted_number.size());
}

extern "C" VALUE rb_phone_number_raw_national(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@raw_national"))) {
    return rb_iv_get(self, "@raw_national");
  }

  VALUE result = format_by_pattern_national(self, raw_national_format);

  return rb_iv_set(self, "@raw_national", result);
}

extern "C" VALUE rb_phone_number_dasherized_national(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@dasherized_national"))) {
    return rb_iv_get(self, "@dasherized_national");
  }

  VALUE result = format_by_pattern_national(self, dasherized_national_format);

  return rb_iv_set(self, "@dasherized_national", result);
}

extern "C" VALUE rb_phone_number_country_code(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@country_code"))) {
    return rb_iv_get(self, "@country_code");
  }

  PhoneNumberInfo *phone_number_info;
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);

  int code = phone_number_info->phone_number->country_code();

  VALUE result = INT2NUM(code);

  return rb_iv_set(self, "@country_code", result);
}

extern "C" VALUE rb_phone_number_dasherized_international(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@dasherized_international"))) {
    return rb_iv_get(self, "@dasherized_international");
  }

  VALUE national = rb_phone_number_dasherized_national(self);
  VALUE cc = rb_fix2str(rb_phone_number_country_code(self), 10);
  VALUE dash = rb_str_new("-", 1);
  VALUE prefix = rb_str_concat(cc, dash);
  VALUE result = rb_str_concat(prefix, national);

  return rb_iv_set(self, "@dasherized_international", result);
}

extern "C" VALUE rb_phone_number_raw_international(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@raw_international"))) {
    return rb_iv_get(self, "@raw_international");
  }

  VALUE national = rb_phone_number_raw_national(self);
  VALUE cc = rb_fix2str(rb_phone_number_country_code(self), 10);
  VALUE result = rb_str_concat(cc, national);

  return rb_iv_set(self, "@raw_international", result);
}

extern "C" VALUE rb_phone_number_possible_eh(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@possible"))) {
    return rb_iv_get(self, "@possible");
  }

  std::string formatted_number;
  PhoneNumberInfo *phone_number_info;
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);

  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

  if (phone_util.IsPossibleNumber(*phone_number_info->phone_number)) {
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
  VALUE input_region_code = rb_iv_get(self, "@input_region_code");

  if (NIL_P(input_region_code)) {
    PhoneNumberInfo *phone_number_info;
    std::string code;
    TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);
    const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

    phone_util.GetRegionCodeForCountryCode(phone_number_info->phone_number->country_code(), &code);

    VALUE result = rb_str_new(code.c_str(), code.size());

    return rb_iv_set(self, "@region_code", result);
  } else {
    return rb_iv_set(self, "@region_code", input_region_code);
  }
}

extern "C" VALUE rb_phone_number_match_eh(VALUE self, VALUE other) {
  if (!rb_obj_is_kind_of(other, rb_cPhoneNumber)) {
    return Qfalse;
  }

  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

  PhoneNumberInfo *self_info;
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, self_info);

  PhoneNumberInfo *other_info;
  TypedData_Get_Struct(other, PhoneNumberInfo, &phone_number_info_type, other_info);

  if (self_info->phone_number->raw_input() == other_info->phone_number->raw_input()) {
    return Qtrue;
  }

  if (phone_util.IsNumberMatch(*other_info->phone_number, *self_info->phone_number) == PhoneNumberUtil::EXACT_MATCH) {
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
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);
  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

  VALUE result;

  // @see
  // https://github.com/google/libphonenumber/blob/4e9954edea7cf263532c5dd3861a801104c3f012/cpp/src/phonenumbers/phonenumberutil.h#L91
  switch (phone_util.GetNumberType(*phone_number_info->phone_number)) {
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

extern "C" VALUE rb_phone_number_area_code(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@area_code"))) {
    return rb_iv_get(self, "@area_code");
  }

  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());
  PhoneNumberInfo *phone_number_info;
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);

  PhoneNumber *number = phone_number_info->phone_number;
  std::string national_significant_number;
  phone_util.GetNationalSignificantNumber(*number, &national_significant_number);
  std::string area_code;
  std::string subscriber_number;

  int area_code_length = phone_util.GetLengthOfGeographicalAreaCode(*number);
  if (area_code_length > 0) {
    area_code = national_significant_number.substr(0, area_code_length);
    subscriber_number = national_significant_number.substr(area_code_length, std::string::npos);
  } else {
    area_code = "";
    subscriber_number = national_significant_number;
  }

  VALUE result = rb_str_new(area_code.c_str(), area_code.size());

  return rb_iv_set(self, "@area_code", result);
}

extern "C" VALUE rb_phone_number_to_s(VALUE self) {
  PhoneNumberInfo *phone_number_info;
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);
  PhoneNumber *phone_number = phone_number_info->phone_number;

  if (phone_number == NULL) {
    return Qnil;
  }

  std::string raw_input = phone_number->raw_input();

  return rb_str_new(raw_input.c_str(), raw_input.size());
}

static inline void setup_formats() {
  // Raw
  NumberFormat *raw_fmt = raw_national_format.Add();
  raw_fmt->set_pattern("(\\d{3})(\\d{3})(\\d{4})");
  raw_fmt->set_format("$1$2$3");

  // Dasherized
  NumberFormat *dsh_fmt = dasherized_national_format.Add();
  dsh_fmt->set_pattern("(\\d{3})(\\d{3})(\\d{4})");
  dsh_fmt->set_format("$1-$2-$3");
}

extern "C" VALUE rb_phone_number_valid_eh(VALUE self) {
  if (rb_ivar_defined(self, rb_intern("@valid"))) {
    return rb_iv_get(self, "@valid");
  }

  std::string formatted_number;
  PhoneNumberInfo *phone_number_info;
  VALUE input_region_code = rb_iv_get(self, "@input_region_code");
  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);

  if (NIL_P(input_region_code)) {
    input_region_code = rb_iv_get(rb_mMiniPhone, "@default_country");
  }

  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

  if (!rb_str_equal(input_region_code, rb_str_new_literal("ZZ"))) {
    std::string country_code(RSTRING_PTR(input_region_code), RSTRING_LEN(input_region_code));

    if (phone_util.IsValidNumberForRegion(*phone_number_info->phone_number, country_code)) {
      return rb_iv_set(self, "@valid", Qtrue);
    } else {
      return rb_iv_set(self, "@valid", Qfalse);
    }
  }

  if (phone_util.IsValidNumber(*phone_number_info->phone_number)) {
    return rb_iv_set(self, "@valid", Qtrue);
  } else {
    return rb_iv_set(self, "@valid", Qfalse);
  }
}

extern "C" VALUE rb_phone_number_invalid_eh(VALUE self) {
  return rb_phone_number_valid_eh(self) == Qtrue ? Qfalse : Qtrue;
}

extern "C" VALUE rb_phone_number_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE str;
  VALUE input_region_code;

  rb_scan_args(argc, argv, "11", &str, &input_region_code);
  rb_iv_set(self, "@input_region_code", input_region_code);

  if (NIL_P(input_region_code)) {
    input_region_code = rb_iv_get(rb_mMiniPhone, "@default_country");
  }

  if (FIXNUM_P(str)) {
    str = rb_fix2str(str, 10);
  } else if (!RB_TYPE_P(str, T_STRING)) {
    return rb_phone_number_nullify_ivars(self);
  }

  PhoneNumberInfo *phone_number_info;
  PhoneNumber parsed_number;

  TypedData_Get_Struct(self, PhoneNumberInfo, &phone_number_info_type, phone_number_info);

  const PhoneNumberUtil &phone_util(*PhoneNumberUtil::GetInstance());

  std::string phone_number(RSTRING_PTR(str), RSTRING_LEN(str));
  std::string country_code(RSTRING_PTR(input_region_code), RSTRING_LEN(input_region_code));

  auto result = phone_util.ParseAndKeepRawInput(phone_number, country_code, &parsed_number);

  if (result != PhoneNumberUtil::NO_PARSING_ERROR) {
    rb_phone_number_nullify_ivars(self);
  }

  phone_number_info->phone_number->Swap(&parsed_number);

  return self;
}

extern "C" void Init_mini_phone(void) {
  setup_formats();

  rb_mMiniPhone = rb_define_module("MiniPhone");

  // Unknown
  rb_iv_set(rb_mMiniPhone, "@default_country", rb_str_new("ZZ", 2));

  rb_define_module_function(rb_mMiniPhone, "valid?", reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_valid), 1);
  rb_define_module_function(rb_mMiniPhone, "valid_for_country?",
                            reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_valid_for_country), 2);
  rb_define_module_function(rb_mMiniPhone, "invalid?", reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_invalid), 1);
  rb_define_module_function(rb_mMiniPhone, "invalid_for_country?",
                            reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_invalid_for_country), 2);
  rb_define_module_function(rb_mMiniPhone, "possible?", reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_valid), 1);
  rb_define_module_function(rb_mMiniPhone, "impossible?", reinterpret_cast<VALUE (*)(...)>(rb_is_phone_number_invalid),
                            1);
  rb_define_module_function(rb_mMiniPhone, "default_country=", reinterpret_cast<VALUE (*)(...)>(rb_set_default_country),
                            1);
  rb_define_module_function(rb_mMiniPhone, "default_country", reinterpret_cast<VALUE (*)(...)>(rb_get_default_country),
                            0);
  rb_define_module_function(rb_mMiniPhone, "parse", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_parse), -1);
  rb_define_module_function(rb_mMiniPhone, "normalize_digits_only",
                            reinterpret_cast<VALUE (*)(...)>(rb_normalize_digits_only), 1);

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

  rb_define_method(rb_cPhoneNumber, "raw_national", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_raw_national), 0);
  rb_define_method(rb_cPhoneNumber, "raw_international",
                   reinterpret_cast<VALUE (*)(...)>(rb_phone_number_raw_international), 0);
  rb_define_method(rb_cPhoneNumber, "dasherized_international",
                   reinterpret_cast<VALUE (*)(...)>(rb_phone_number_dasherized_international), 0);
  rb_define_method(rb_cPhoneNumber, "dasherized_national",
                   reinterpret_cast<VALUE (*)(...)>(rb_phone_number_dasherized_national), 0);
  rb_define_method(rb_cPhoneNumber, "international", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_international),
                   0);
  rb_define_method(rb_cPhoneNumber, "rfc3966", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_rfc3966), 0);
  rb_define_method(rb_cPhoneNumber, "region_code", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_region_code), 0);
  rb_define_method(rb_cPhoneNumber, "country", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_region_code), 0);
  rb_define_method(rb_cPhoneNumber, "country_code", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_country_code), 0);
  rb_define_method(rb_cPhoneNumber, "type", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_type), 0);
  rb_define_method(rb_cPhoneNumber, "area_code", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_area_code), 0);
  rb_define_method(rb_cPhoneNumber, "to_s", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_to_s), 0);
  rb_define_method(rb_cPhoneNumber, "==", reinterpret_cast<VALUE (*)(...)>(rb_phone_number_match_eh), 1);
}
