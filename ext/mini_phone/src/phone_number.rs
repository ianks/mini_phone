use std::str::FromStr;

use magnus::{
    function, method,
    module::Module,
    r_string::IntoRString,
    typed_data::Obj,
    value::{InnerValue, Lazy, LazyId, Opaque, ReprValue},
    DataTypeFunctions, IntoValue, Object, RClass, RString, Ruby, StaticSymbol, TryConvert,
    TypedData, Value,
};
use phonenumber::PhoneNumber;

use crate::root_module;

#[derive(TypedData)]
#[magnus(class = "MiniPhone::PhoneNumber", free_immediately, size, mark)]
pub struct RubyPhoneNumber {
    #[magnus(opaque_attr_reader)]
    input: Opaque<Value>,
    inner: Option<PhoneNumber>,
}

impl DataTypeFunctions for RubyPhoneNumber {
    fn size(&self) -> usize {
        std::mem::size_of::<Self>()
    }

    fn mark(&self, #[allow(unused_variables)] marker: &magnus::gc::Marker) {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        marker.mark(self.input.get_inner_with(&ruby));
    }
}

impl RubyPhoneNumber {
    pub fn new(input: Value) -> Result<Self, magnus::Error> {
        let phone_number = RString::try_convert(input)
            .map(|s| {
                if let Ok(s) = unsafe { s.as_str() } {
                    PhoneNumber::from_str(s).ok()
                } else {
                    None
                }
            })
            .unwrap_or(None);

        Ok(Self {
            input: Opaque::from(input),
            inner: phone_number,
        })
    }

    pub fn database() -> &'static phonenumber::metadata::Database {
        &phonenumber::metadata::DATABASE
    }

    pub fn is_valid(&self) -> bool {
        if let Some(phone_number) = &self.inner {
            phonenumber::is_valid(phone_number)
        } else {
            false
        }
    }

    pub fn e164(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        self.inner
            .as_ref()
            .map(|p| p.format().to_string().into_r_string().into_value())
            .unwrap_or(ruby.qnil().into_value())
    }

    pub fn country_code(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        self.inner
            .as_ref()
            .map(|p| p.country().code().into_value())
            .unwrap_or(ruby.qnil().into_value())
    }

    pub fn is_equal(&self, other: Value) -> bool {
        if let Ok(other) = Obj::<Self>::try_convert(other) {
            if self.input().equal(other.input()).unwrap_or(false) {
                true
            } else if let (Some(self_phone_number), Some(other_phone_number)) =
                (&self.inner, &other.inner)
            {
                self_phone_number == other_phone_number
            } else {
                false
            }
        } else {
            false
        }
    }

    pub fn as_symbol(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");

        if let Some(phone_number) = &self.inner {
            match phone_number.number_type(Self::database()) {
                phonenumber::Type::FixedLine => {
                    static SYM: LazyId = LazyId::new("fixed_line");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::Mobile => {
                    static SYM: LazyId = LazyId::new("mobile");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::FixedLineOrMobile => {
                    static SYM: LazyId = LazyId::new("fixed_line_or_mobile");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::TollFree => {
                    static SYM: LazyId = LazyId::new("toll_free");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::PremiumRate => {
                    static SYM: LazyId = LazyId::new("premium_rate");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::SharedCost => {
                    static SYM: LazyId = LazyId::new("shared_cost");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::Voip => {
                    static SYM: LazyId = LazyId::new("voip");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::PersonalNumber => {
                    static SYM: LazyId = LazyId::new("personal_number");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::Pager => {
                    static SYM: LazyId = LazyId::new("pager");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::Uan => {
                    static SYM: LazyId = LazyId::new("uan");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::Voicemail => {
                    static SYM: LazyId = LazyId::new("voicemail");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::Unknown => {
                    static SYM: LazyId = LazyId::new("unknown");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::Emergency => {
                    static SYM: LazyId = LazyId::new("emergency");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::ShortCode => {
                    static SYM: LazyId = LazyId::new("short_code");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::StandardRate => {
                    static SYM: LazyId = LazyId::new("standard_rate");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::Carrier => {
                    static SYM: LazyId = LazyId::new("carrier");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
                phonenumber::Type::NoInternational => {
                    static SYM: LazyId = LazyId::new("no_international");
                    StaticSymbol::from(LazyId::get_inner_with(&SYM, &ruby))
                }
            }
            .into_value()
        } else {
            ruby.qnil().into_value()
        }
    }

    pub fn area_code(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        let db = Self::database();
        self.inner
            .as_ref()
            .and_then(|p| p.metadata(db))
            .and_then(|m| m.national_prefix())
            .map(|p| p.into_r_string().into_value())
            .unwrap_or(ruby.qnil().into_value())
    }

    pub fn region_code(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        self.inner
            .as_ref()
            .map(|p| {
                p.country()
                    .id()
                    .map(|id| id.as_ref().into_r_string().into_value())
                    .unwrap_or(ruby.qnil().into_value())
            })
            .unwrap_or(ruby.qnil().into_value())
    }

    pub fn national(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        self.inner
            .as_ref()
            .map(|p| {
                p.format()
                    .mode(phonenumber::Mode::National)
                    .to_string()
                    .into_r_string()
                    .into_value()
            })
            .unwrap_or(ruby.qnil().into_value())
    }

    pub fn country(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        self.inner
            .as_ref()
            .map(|p| {
                p.country()
                    .id()
                    .map(|id| id.as_ref().into_r_string().into_value())
                    .unwrap_or(ruby.qnil().into_value())
            })
            .unwrap_or(ruby.qnil().into_value())
    }

    pub fn rfc3966(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        self.inner
            .as_ref()
            .map(|p| {
                p.format()
                    .mode(phonenumber::Mode::Rfc3966)
                    .to_string()
                    .into_r_string()
                    .into_value()
            })
            .unwrap_or(ruby.qnil().into_value())
    }

    pub fn international(&self) -> Value {
        let ruby = Ruby::get().expect("Ruby interpreter not initialized");
        self.inner
            .as_ref()
            .map(|p| {
                p.format()
                    .mode(phonenumber::Mode::International)
                    .to_string()
                    .into_r_string()
                    .into_value()
            })
            .unwrap_or(ruby.qnil().into_value())
    }
}

pub fn phone_number_class() -> RClass {
    static MODULE: Lazy<RClass> = Lazy::new(|_ruby| {
        root_module()
            .define_class("PhoneNumber", magnus::class::object())
            .expect("PhoneNumber module not defined")
    });
    let ruby = Ruby::get().expect("Ruby interpreter not initialized");
    MODULE.get_inner_with(&ruby)
}

pub fn init() -> Result<(), magnus::Error> {
    let class = phone_number_class();
    class.define_singleton_method("new", function!(RubyPhoneNumber::new, 1))?;
    class.define_singleton_method("parse", function!(RubyPhoneNumber::new, 1))?;
    class.define_method("valid?", method!(RubyPhoneNumber::is_valid, 0))?;
    class.define_method("e164", method!(RubyPhoneNumber::e164, 0))?;
    class.define_method("to_s", method!(RubyPhoneNumber::e164, 0))?;
    class.define_method("country_code", method!(RubyPhoneNumber::country_code, 0))?;
    class.define_method("type", method!(RubyPhoneNumber::as_symbol, 0))?;
    class.define_method("area_code", method!(RubyPhoneNumber::area_code, 0))?;
    class.define_method("region_code", method!(RubyPhoneNumber::region_code, 0))?;
    class.define_method("==", method!(RubyPhoneNumber::is_equal, 1))?;
    class.define_method("national", method!(RubyPhoneNumber::national, 0))?;
    class.define_method("country", method!(RubyPhoneNumber::country, 0))?;
    class.define_method("rfc3966", method!(RubyPhoneNumber::rfc3966, 0))?;
    class.define_method("international", method!(RubyPhoneNumber::international, 0))?;

    Ok(())
}
